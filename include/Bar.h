/**
 * @file    Bar.h
 * @brief   Defines the Bar class representing a portion of market data for an equity.
 *
 * This file contains the declaration of the Bar class, which stores first, 
 * last, min, and max prices, for an equity price type (like bid, ask, or trade).
 * It is intended to be a member of an EquitySnapshot defined in EquitySnapshot.h.
 *
 * @author  Benny Zaionz
 * @date    2025-06-18
 * @version 1.0
 */


#ifndef BAR_H
#define BAR_H

#include <string>

namespace AlgoTrading
{

// enum PrintType { TRADE, BID, ASK };

class Bar
{
    private:

        double first;
        double last;
        double low;
        double high;


    public:

        /*---------- CONSTRUCTOR ----------*/
        Bar(const double first_ = -1, 
            const double last_ = -1, 
            const double low_ = -1,  
            const double high_ = -1);

        /*---------- GETTERS ----------*/ 
        
        double getFirst() const { return first; }
        double getLast() const { return last; }
        double getLow() const { return low; }
        double getHigh() const { return high; }
        
        /* ---------- SETTERS ---------*/
        
        void setFirst(const double first_) { first = first_; }
        void setLast(const double last_) { last = last_; }
        void setLow(const double low_) { low = low_; }
        void setHigh(const double high_) { high = high_; }
        
        /*---------- PRINT HELPER ----------*/

        void print(const int print_type) const;
};

} // namespace

#endif // EQUITY_SNAPSHOT_H