/**
 * @file    EquitySnapshot.h
 * @brief   Defines the EquitySnapshot class representing a snapshot of data for an equity at a single point in time.
 *
 * This file contains the declaration of the EquitySnapshot class, which stores last, low, high, bid, and ask prices,
 * as well as volume for an equity at a specific point in time. 
 * It is used by the AlgoTrading system for both backtesting and live trading data processing.
 *
 * @author  Benny Zaionz
 * @date    2025-06-18
 * @version 1.0
 */


#ifndef EQUITY_SNAPSHOT_H
#define EQUITY_SNAPSHOT_H

#include <string>

#include "core/DateTime.h"

namespace AlgoTrading
{

enum class PrintType { TRADE, BID_ASK };
enum class PriceType { LAST, LOW, HIGH, BID, ASK };
enum class UpdateType { SUCCESSFUL_UPDATE, TICKER_NOT_IN_PORTFOLIO };

const int DOES_NOT_CONTAIN = -1; // for containsTicker()

class EquitySnapshot
{
    private:

        DateTime dt;
        double last;
        double low;
        double high;
        double bid;
        double ask;
        int volume;

    public:

        /*---------- CONSTRUCTOR ----------*/

        EquitySnapshot(); 

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

        DateTime getDateTime() const { return dt; }

        double getLast() const { return last; }
        double getLow() const { return low; }
        double getHigh() const { return high; }
        double getBid() const { return bid; }
        double getAsk() const { return ask; }
        double getPrice(const PriceType price_type = PriceType::LAST) const;

        int getVolume() const { return volume; }

        
        /* ---------- SETTERS ---------*/
    
        void setLast(const double last_) { last = last_; }
        void setLow(const double low_) { low = low_; }
        void setHigh(const double high_) { high = high_; }
        void setBid(const double avg_bid_) { bid = avg_bid_; }
        void setAsk(const double avg_ask_) { ask = avg_ask_; }
        
        void setDateTime(const DateTime& dt_) {dt = dt_;}

        void setVolume(const int volume_) { volume = volume_; }
        
        /*---------- PRINT HELPER ----------*/

        void print(const PrintType print_type = PrintType::BID_ASK) const;
};

} // namespace

#endif // EQUITY_SNAPSHOT_H