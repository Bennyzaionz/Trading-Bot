#include "LiveEquity.h"

namespace AlgoTrading
{

LiveEquity::LiveEquity(const std::string& ticker_, const EquitySnapshot& snap):
ticker(ticker_), currentSnapshot(snap) {}

void LiveEquity::print(int print_type) const
{
    std::cout << "Ticker: " << getTicker() << ", ";

    currentSnapshot.print(print_type);

    // std::cout << std::endl;
}

}