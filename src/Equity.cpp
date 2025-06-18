#include "Equity.h"

namespace AlgoTrading
{

Equity::Equity(const std::string ticker_, 
               const double bid_ , const double ask_ , const double last_ , 
               const double high_ , const double low_ , const double open_ , const double close_):
ticker(ticker_), bid(bid_), ask(ask_), last(last_), high(high_), low(low_), open(open_), close(close_) {}

void Equity::print(int print_type) const
{
    std::cout << "Ticker: " << ticker << ", ";
    if(print_type == OHLC)
        std::cout << "Open: " << open << ", Close: " << close << ", Low: " << low << ", High: " << high; // << std::endl;
    else if(print_type == BID_ASK) 
        std::cout << "Bid: " << bid << ", Ask: " << ask;// << std::endl;
    else
        std::cout << " print_type " << print_type << " is unknown";// << std::endl;
}

}