/*
*/

#ifndef LIVE_EQUITY_H
#define LIVE_EQUITY_H

#include <string>
#include <iostream>
#include "EquitySnapshot.h"

namespace AlgoTrading
{

class LiveEquity
{
    private:

        const std::string ticker;
        EquitySnapshot currentSnapshot;

    public:

        /*---------- CONSTRUCTOR ----------*/

        LiveEquity(const std::string& ticker_ = "", const EquitySnapshot& snap = EquitySnapshot());

        /*---------- GETTERS ----------*/

        std::string getTicker() const { return ticker; }
        DateTime getDatetime() const { return currentSnapshot.getDatetime(); }

        double getLast() const { return currentSnapshot.getLast(); }
        double getLow() const { return currentSnapshot.getLow(); }
        double getHigh() const { return currentSnapshot.getHigh(); }
        double getBid() const { return currentSnapshot.getBid(); }
        double getAsk() const { return currentSnapshot.getAsk(); }
        double getVolume() const { return currentSnapshot.getVolume(); }
        EquitySnapshot getCurrentSnapshot() const { return currentSnapshot; }

        /*---------- SETTERS ----------*/

        void setDatetime(const std::string& datetime_) { currentSnapshot.setDatetime(datetime_); }
        void setDatetime(const DateTime& datetime_) { currentSnapshot.setDatetime(datetime_); }
        void setLast(const double last_) { currentSnapshot.setLast(last_); }
        void setLow(const double low_) { currentSnapshot.setLow(low_); }
        void setHigh(const double high_) { currentSnapshot.setHigh(high_); }
        void setBid(const double bid_) { currentSnapshot.setBid(bid_); }
        void setAsk(const double ask_) { currentSnapshot.setAsk(ask_); }

        /*---------- PRINT HELPER ----------*/

        void print(int print_type = BID_ASK) const;

};

} // end 

#endif