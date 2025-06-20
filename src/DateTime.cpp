/**
 * @file    DateTime.cpp
 * @brief   Defines the DateTime class functionality.
 *
 * This file contains the functions defining the constructors for the 
 * DateTime class, as well as some useful functions including operator
 * overload for adding and subtracting dates.
 *
 * @author  Benny Zaionz
 * @date    2025-06-19
 * @version 1.0
 */

 #include <stdexcept>
 #include <iostream>
 #include <sstream>
 #include <iomanip>

 #include "DateTime.h"

 namespace AlgoTrading
 {

 DateTime::DateTime(const int year_,
                    const int month_,
                    const int day_,
                    const int hour_,
                    const int min_,
                    const int sec_):
    year(year_), month(month_), day(day_), hour(hour_), min(min_), sec(sec_) {}

DateTime::DateTime(const std::string& datetime_)
{
    /*
    if using the string version of the constructor, be careful
    as it does not check for string formatting and validity
    Assumes datetime_ is either "YYYYMMDD" or "YYYYMMDD hh:mm:ss"
    */
    if( datetime_.size() == 8)
    {
        year  = std::stoi(datetime_.substr(0, 4));
        month = std::stoi(datetime_.substr(4, 2));
        day   = std::stoi(datetime_.substr(6, 2));
        hour  = 0;
        min   = 0;
        sec   = 0;
    }

    else if( datetime_.size() == 17)
    {
        year  = std::stoi(datetime_.substr(0, 4));
        month = std::stoi(datetime_.substr(4, 2));
        day   = std::stoi(datetime_.substr(6, 2));
        hour  = std::stoi(datetime_.substr(9, 10));
        min   = std::stoi(datetime_.substr(12, 13));
        sec   = std::stoi(datetime_.substr(15, 16));        
    }

    else
        throw::std::invalid_argument("Date string must be in YYYYMMDD format");
}    

DateTime DateTime::operator+(int secondsToAdd) const 
{
    
    /*
    Since the twsapi only requires time data for time steps of
    hr, min, and sec, this operator will only handle the logic for 
    rolling over those values during market open time (i.e no
    need to handle day rollover, month rollover, or year rollover)
    */

    int updated_sec = sec + secondsToAdd;
    // handle hour rollover
    int updated_hour = hour + updated_sec / 3600;
    updated_sec = updated_sec % 3600;

    // handle minute rollover
    int updated_min = min + updated_sec / 60;
    updated_sec = updated_sec % 60;

    return DateTime(year, month, day, updated_hour, updated_min, updated_sec);
}

DateTime& DateTime::operator+=(const int secondsToAdd) {
    std::tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = min;
    t.tm_sec  = sec + secondsToAdd;

    std::time_t normalized = std::mktime(&t);
    if (normalized == -1) {
        throw std::runtime_error("Invalid time during += operation");
    }

    // Update internal state
    year  = t.tm_year + 1900;
    month = t.tm_mon + 1;
    day   = t.tm_mday;
    hour  = t.tm_hour;
    min   = t.tm_min;
    sec   = t.tm_sec;

    return *this;
}

bool DateTime::operator==(const DateTime& datetime_)
{
    return ( getYear() == datetime_.getYear() &&
             getMonth() == datetime_.getMonth() &&
             getDay() == datetime_.getDay() &&
             getHour() == datetime_.getHour() &&
             getMin() == datetime_.getMin() &&
             getSec() == datetime_.getSec() );
}

bool DateTime::sameDateAs(const DateTime& datetime_) const
{
    return ( getYear() == datetime_.getYear() && 
             getMonth() == datetime_.getMonth() &&
             getDay() == datetime_.getDay() );
}

std::string DateTime::toString() const
{
    std::stringstream ss;

    ss << std::setw(4) << std::setfill('0') << year << "-"
       << std::setw(2) << std::setfill('0') << month << "-"
       << std::setw(2) << std::setfill('0') << day << " "
       << std::setw(2) << std::setfill('0') << hour << ":"
       << std::setw(2) << std::setfill('0') << min << ":"
       << std::setw(2) << std::setfill('0') << sec;

    return ss.str();
}

void DateTime::print() const
{
    std::cout << toString() << std::endl;;
}

} // end of namespace