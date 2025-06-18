#include "EquitySnapshot.h"

#include <string>
#include <iostream>

namespace AlgoTrading
{

/*---------- CONSTRUCTOR ----------*/
EquitySnapshot::EquitySnapshot
(
    const std::string datetime_,
    double open_,
    double close_,
    double low_,
    double high_,
    double avg_bid_,
    double avg_ask_,
    double min_bid_,
    double max_ask_,
    int volume_
):
    datetime(datetime_),
    open(open_),
    close(close_),
    low(low_),
    high(high_),
    avg_bid(avg_bid_),
    avg_ask(avg_ask_),
    min_bid(min_bid_),
    max_ask(max_ask_),
    volume(volume_) {} 

/*---------- PRINTING HELPERS ----------*/

void EquitySnapshot::print(const std::string print_type)
{
    if ( print_type == "OHLC" )
        std::cout << "Datetime: " << datetime << ", Open: " << open << ", Close: " << close << ", Low: " << low << ", High:" << high << std::endl;
    else if ( print_type == "OHLC/V")
        std::cout << "Datetime: " << datetime<< ", Open: " << open << ", Close: " << close << ", Low: " << low << ", High:" << high << ", Volume: " << volume << std::endl;
    else if ( print_type == "BID-ASK")
        std::cout << "Datetime: " << datetime<< ", Avg Bid: " << avg_bid << ", Avg Ask: " << avg_ask << ", Min Bid: " << min_bid << ", Max Ask:" << max_ask << std::endl;

    else
        std::cout << "Print Type " << print_type << " does not exist" << std::endl;
}

} // namespace