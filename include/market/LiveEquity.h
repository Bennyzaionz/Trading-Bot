/*
*/

#ifndef LIVE_EQUITY_H
#define LIVE_EQUITY_H

#include <string>
#include <iostream>
#include "core/EquitySnapshot.h"

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

        /*---------- OPERATORS ----------*/
        // LiveEquity& operator=(LiveMarket& other);

        /*---------- GETTERS ----------*/

        std::string getTicker() const { return ticker; }
        DateTime getDatetime() const { return currentSnapshot.getDateTime(); }

        double getOpen() const { return currentSnapshot.getOpen(); }
        double getClose() const { return currentSnapshot.getClose(); }
        double getLast() const { return currentSnapshot.getLast(); }
        double getLow() const { return currentSnapshot.getLow(); }
        double getHigh() const { return currentSnapshot.getHigh(); }
        double getBid() const { return currentSnapshot.getBid(); }
        double getAsk() const { return currentSnapshot.getAsk(); }
        double getMarketValue() const { return (currentSnapshot.getAsk() + currentSnapshot.getBid() )/2; }
        double getVolume() const { return currentSnapshot.getVolume(); }
        EquitySnapshot getCurrentSnapshot() const { return currentSnapshot; }

        /*---------- SETTERS ----------*/
    
        void setDatetime(const DateTime& datetime_) { currentSnapshot.setDateTime(datetime_); }
        void setOpen(const double open_) { currentSnapshot.setOpen(open_); }
        void setClose(const double close_) { currentSnapshot.setClose(close_); }
        void setLast(const double last_) { currentSnapshot.setLast(last_); }
        void setLow(const double low_) { currentSnapshot.setLow(low_); }
        void setHigh(const double high_) { currentSnapshot.setHigh(high_); }
        void setBid(const double bid_) { currentSnapshot.setBid(bid_); }
        void setAsk(const double ask_) { currentSnapshot.setAsk(ask_); }
        void setVolume(const int volume_) { currentSnapshot.setVolume(volume_ ); }

        /*---------- UPDATE ----------*/

        void updateEquitySnapshot(const double open_,
                                  const double close_,
                                  const double last_,
                                  const double low_,
                                  const double high_,
                                  const double bid_,
                                  const double ask_,
                                  const int volume_,
                                  const DateTime& dt_);

        /*---------- PRINT HELPER ----------*/

        void print(const PrintType print_type = PrintType::BID_ASK) const;

};

} // end 

#endif