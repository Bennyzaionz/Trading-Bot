/**
 * @file    Bar.cpp
 * @brief   Defines the Bar class functionality.
 *
 * This file contains the constructor for the Bar class. 
 * It also defines a printing function which can be used to easily 
 * visualize and debug. It is used by the AlgoTrading system for both
 * backtesting and live trading data processing.
 *
 * @author  Benny Zaionz
 * @date    2025-06-18
 * @version 1.0
 */

#include "Bar.h"

#include <iostream>

namespace AlgoTrading
{

/*---------- CONSTRUCTOR ----------*/

Bar::Bar(double first_, double last_, double low_, double high_): 
first(first_), last(last_), low(low_), high(high_) {} 

/*---------- PRINTING HELPERS ----------*/

void Bar::print(const int print_type) const
{
    std::string type;

    if( print_type == TRADE)
        type = "Trade";
    else if( print_type == BID)
        type = "Bid";
    else if( print_type == ASK)
        type = "Ask";
    else    
        type = "UNKNOWN";

    std::cout << ", First " << type << ": " << getFirst() 
              << ", Last: " << type << ": " << getLast() 
              << ", Low: " << type << ": " << getLow() 
              << ", High:" << type << ": " << getHigh()
              << std::endl;

}

} // namespace