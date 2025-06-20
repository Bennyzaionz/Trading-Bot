/**
 * @file    EquitySnapshot.h
 * @brief   Defines the EquitySnapshot class representing a snapshot of market data for an equity.
 *
 * This file contains the declaration of the EquitySnapshot class, which stores OHLC prices,
 * bid/ask prices, last traded price, and timestamp information for an equity at a 
 * specific point in time. It is used by the AlgoTrading system for both backtesting 
 * and live trading data processing.
 *
 * @author  Benny Zaionz
 * @date    2025-06-18
 * @version 1.0
 */


#ifndef EQUITY_SNAPSHOT_H
#define EQUITY_SNAPSHOT_H

#include <string>
#include "DateTime.h"

namespace AlgoTrading
{

enum PrintType { TRADE, BID_ASK };
enum PriceType{ LAST, LOW, HIGH, BID, ASK };

class EquitySnapshot
{
    private:

        DateTime dt;
        // std::string datetime;
        double last;
        double low;
        double high;
        double bid;
        double ask;
        int volume;

    public:

        EquitySnapshot(); // constructor with no specified params

        /*---------- CONSTRUCTOR ----------*/
        EquitySnapshot(const std::string& datetime_, 
                       const double last_ = -1,
                       const double low_ = -1,
                       const double high_ = -1,
                       const double bid_ = -1,
                       const double ask_ = -1,
                       const int volume_ = 0);

        EquitySnapshot(const DateTime& datetime_, 
                       const double last_ = -1,
                       const double low_ = -1,
                       const double high_ = -1,
                       const double bid_ = -1,
                       const double ask_ = -1,
                       const int volume_ = 0);                       

        /*---------- GETTERS ----------*/ 
        
        // std::string getDatetime() const { return datetime; }
        DateTime getDatetime() const { return dt; }
        double getLast() const { return last; }
        double getLow() const { return low; }
        double getHigh() const { return high; }
        double getBid() const { return bid; }
        double getAsk() const { return ask; }
        int getVolume() const { return volume; }
        double getPrice(const int price_type = LAST) const;


        
        /* ---------- SETTERS ---------*/
        
        // void setDateTime(const std::string& datetime_) { datetime = datetime_; }
        void setDatetime(const DateTime& datetime_) { dt = datetime_; }
        void setDatetime(const std::string& datetime_) { dt = DateTime(datetime_); }
        void setLast(const double last_) { last = last_; }
        void setLow(const double low_) { low = low_; }
        void setHigh(const double high_) { high = high_; }
        void setBid(const double avg_bid_) { bid = avg_bid_; }
        void setAsk(const double avg_ask_) { ask = avg_ask_; }
        void setVolume(const int volume_) { volume = volume_; }
        
        /*---------- PRINT HELPER ----------*/

        void print(const int print_type = BID_ASK) const;
};

} // namespace

#endif // EQUITY_SNAPSHOT_H