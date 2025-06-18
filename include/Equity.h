#ifndef EQUITY_H
#define EQUITY_H

#include <string>
#include <iostream>
#include "EquitySnapshot.h"

namespace AlgoTrading
{

enum PrintType { OHLC, BID_ASK };

class Equity
{
    private:

        const std::string ticker;
        double bid;
        double ask;
        double last;
        double high;
        double low;
        double open;
        double close;

    public:

    /*---------- CONSTRUCTOR ----------*/

    Equity(const std::string ticker_ = "", 
           const double bid_ = -1, const double ask_ = -1, const double last_ = -1, 
           const double high_ = -1, const double low_ = -1, const double open_ = -1, const double close_ = -1);

    /*---------- GETTERS ----------*/

    std::string getTicker() const { return ticker; }
    double getOpen() const { return open; }
    double getClose() const { return close; }
    double getLow() const { return low; }
    double getHigh() const { return high; }
    double getBid() const { return bid; }
    double getAsk() const { return ask; }
    double getLast() const { return last; }

    /*---------- SETTERS ----------*/

    void setBid(double bid_) { bid = bid_; }
    void setAsk(double ask_) { ask = ask_; }
    void setLast(double last_) { last = last_; }
    void setHigh(double high_) { high = high_; }
    void setLow(double low_) { low = low_; }
    void setOpen(double open_) { open = open_; }
    void setClose(double close_) { close = close_; }

    /*---------- PRINT HELPER ----------*/

    void print(int print_type = BID_ASK) const;



};

} // end 

#endif