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

 #include "core/DateTime.h"

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

// DateTime DateTime::operator+(int secondsToAdd) const 
// {
    
//     /*
//     Since the twsapi only requires time data for time steps of
//     hr, min, and sec, this operator will only handle the logic for 
//     rolling over those values during market open time (i.e no
//     need to handle day rollover, month rollover, or year rollover)
//     */

//     int updated_sec = sec + secondsToAdd;
//     // handle hour rollover
//     int updated_hour = hour + updated_sec / 3600;
//     updated_sec = updated_sec % 3600;

//     // handle minute rollover
//     int updated_min = min + updated_sec / 60;
//     updated_sec = updated_sec % 60;

//     return DateTime(year, month, day, updated_hour, updated_min, updated_sec);
// }

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

bool isLeapYear(int year) {
    // Leap year if divisible by 400 or divisible by 4 but not 100
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int year, int month) {
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28;
        default:
            return 30; // fallback, should not happen
    }
}

DateTime DateTime::operator+(int secondsToAdd) const 
{
    int total_seconds = sec + secondsToAdd;
    int updated_sec = total_seconds % 60;
    int total_minutes = min + total_seconds / 60;
    int updated_min = total_minutes % 60;
    int total_hours = hour + total_minutes / 60;
    int updated_hour = total_hours % 24;

    // Start with current year, month, day
    int updated_year = year;
    int updated_month = month;
    int updated_day = day + total_hours / 24;  // add overflow days

    // Handle day/month/year rollover
    while (true) {
        int dim = daysInMonth(updated_year, updated_month);
        if (updated_day <= dim) {
            break;
        }
        updated_day -= dim;
        updated_month++;
        if (updated_month > 12) {
            updated_month = 1;
            updated_year++;
        }
    }

    return DateTime(updated_year, updated_month, updated_day, updated_hour, updated_min, updated_sec);
}


bool DateTime::operator==(const DateTime& datetime_) const
{
    return ( getYear() == datetime_.getYear() &&
             getMonth() == datetime_.getMonth() &&
             getDay() == datetime_.getDay() &&
             getHour() == datetime_.getHour() &&
             getMin() == datetime_.getMin() &&
             getSec() == datetime_.getSec() );
}

bool DateTime::operator<(const DateTime& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    if (day != other.day) return day < other.day;
    if (hour != other.hour) return hour < other.hour;
    if (min != other.min) return min < other.min;
    return sec < other.sec;
}

bool DateTime::operator>(const DateTime& other) const {
    return other < *this;
}


// bool DateTime::sameDateAs(const DateTime& datetime_) const
// {
//     return ( getYear() == datetime_.getYear() && 
//              getMonth() == datetime_.getMonth() &&
//              getDay() == datetime_.getDay() );
// }

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

DateTime getCurrentDateTime()
{
    // Get current time point
    auto now = std::chrono::system_clock::now();

    // Convert to time_t
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert to tm struct (local time)
    std::tm local_tm = *std::localtime(&now_c);

    // Construct your DateTime object
    return DateTime(local_tm.tm_year + 1900,
                    local_tm.tm_mon + 1,
                    local_tm.tm_mday,
                    local_tm.tm_hour,
                    local_tm.tm_min,
                    local_tm.tm_sec);
}

} // end of namespace