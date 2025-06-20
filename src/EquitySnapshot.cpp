/**
 * @file    EquitySnapshot.cpp
 * @brief   Defines the EquitySnapshot class functionality.
 *
 * This file contains the constructor for the EquitySnapshot class. 
 * It also defines a printing function which can be used to easily 
 * visualize and debug. It is used by the AlgoTrading system for both
 * backtesting and live trading data processing.
 *
 * @author  Benny Zaionz
 * @date    2025-06-18
 * @version 1.0
 */

#include "EquitySnapshot.h"

#include <iostream>

namespace AlgoTrading
{

/*---------- CONSTRUCTOR ----------*/

EquitySnapshot::EquitySnapshot():
dt(DateTime()), last(-1), low(-1), high(-1), bid(-1), ask(-1), volume(0) {} 


EquitySnapshot::EquitySnapshot(const std::string& datetime_,
                               const double last_,
                               const double low_,
                               const double high_,
                               const double bid_,
                               const double ask_,
                               int volume_):
dt(DateTime(datetime_)), last(last_), low(low_), high(high_), bid(bid_), ask(ask_), volume(volume_) {} 

EquitySnapshot::EquitySnapshot(const DateTime& datetime_,
                               const double last_,
                               const double low_,
                               const double high_,
                               const double bid_,
                               const double ask_,
                               int volume_):
dt(datetime_), last(last_), low(low_), high(high_), bid(bid_), ask(ask_), volume(volume_) {} 

/*---------- GETTERS ----------*/

double EquitySnapshot::getPrice(const int price_type) const
{
    if( price_type == LAST )
        return getLast();
    else if( price_type == LOW)
        return getLow();
    else if( price_type == HIGH )
        return getHigh();
    else if( price_type == BID )
        return getBid();
    else if( price_type == ASK )
        return getAsk();
    
    return -1;
}

/*---------- PRINTING HELPERS ----------*/

void EquitySnapshot::print(const int print_type) const
{

    // std::cout << "---------- EquitySnapshot ----------" << std::endl;

    if ( print_type == TRADE )
    {

        std::cout << "Datetime: " << getDatetime().toString() 
                  << ", Last: " << getLast() 
                  << ", Low: " << getLow() 
                  << ", High:" << getHigh() 
                  << ", Volume: " << getVolume(); // << std::endl;
    }

    else if ( print_type == BID_ASK)
    {

        std::cout << "Datetime: " << getDatetime().toString()
                  << ", Bid: " << getBid() 
                  << ", Ask: " << getAsk();  
                  //<< std::endl;
    
}
    else
        std::cout << "Print Type " << print_type << " does not exist" << std::endl;

    // std::cout << "------------------------------------" << std::endl;

}

} // namespace