/**
 * @file    DateTime.h
 * @brief   Defines the DateTime class.
 *
 * This file contains the declaration of the DateTime class, which stores year,
 * month, day, hour, minute, and second. It is intended to be used by the EquitySnapshot 
 * class. 
 *
 * @author  Benny Zaionz
 * @date    2025-06-19
 * @version 1.0
 */

 #ifndef DATETIME_H
 #define DATETIME_H

 #include <string>
 #include <ctime>

namespace AlgoTrading
{

class DateTime
{

    private:

        int year;
        int month;
        int day;
        int hour;
        int min;
        int sec;

    public:

        /*---------- CONSTRUCTORS ----------*/

        DateTime(const std::string& datetime_);

        DateTime(const int year_ = 2000,
                 const int month_ = 1,
                 const int day_ = 1,
                 const int hour_ = 0,
                 const int min_ = 0,
                 const int sec_ = 0);
        
        /*---------- GETTERS ----------*/
        
        int getYear() const { return year; }
        int getMonth() const { return month; }
        int getDay() const { return day; }
        int getHour() const { return hour; }
        int getMin() const { return min; }
        int getSec() const { return sec; }  


        /*---------- SETTERS ----------*/

        void setYear(int year_) { year = year_; }
        void setMonth(int month_) { month = month_; }
        void setDay(int day_) { day = day_; }
        void setHour(int hour_) { hour = hour_; }
        void setMin(int min_) { min = min_; }
        void setSec(int sec_) { sec = sec_; }

        /*---------- OPERATOR OVERLOAD ----------*/

        DateTime operator+(const int secondsToAdd) const;
        DateTime& operator+=(const int secondsToAdd);
        bool operator==(const DateTime& datetime_);

        /*---------- COMPARISONS ----------*/

        bool sameDateAs(const DateTime& datetime_) const;

        /*---------- PRINT HELPERS ----------*/

        std::string toString() const;
        void print() const;
};

} // end namespace

 #endif