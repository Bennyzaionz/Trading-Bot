/**
 * @file    HistoricalMarketData.cpp
 * @brief   Defines the HistoricalMarketData class functionality.
 *
 * HistoricalMarketData is a collection of HistoricalEquityData objects
 * and is intended to be used to collect the history of many stocks.
 * This file includes defintions for the constrcutors, 
 * 
 *
 * @author  Benny Zaionz
 * @date    2025-06-20
 * @version 1.0
 */

#include <stdexcept>
#include <algorithm>
#include "HistoricalMarketData.h"

namespace AlgoTrading
{
 
HistoricalMarketData::HistoricalMarketData():
hist_equities{} {}

// HistoricalMarketData::HistoricalMarketData(std::vector<std::string> tickers):
// {

// }

std::vector<std::string> HistoricalMarketData::getTickers() const
{
    
    std::vector<HistoricalEquityData> histories = getHistories();

    std::vector<std::string> tickers;

    for( int i = 0; i < getSize(); i++)
        tickers.push_back(histories[i].getTicker());

    return tickers;
}

HistoricalEquityData HistoricalMarketData::getHistory(const std::string& ticker) const
{
    int index = containsTicker(ticker);

    if( index == DOES_NOT_CONTAIN)
        throw std::runtime_error("Ticker not found in Historical Market");

    return getHistories()[index];
}

int HistoricalMarketData::containsTicker(const std::string ticker) const
{
    std::vector<std::string> holdings = getTickers();

    auto it = std::find(holdings.begin(), holdings.end(), ticker);

    if( it != holdings.end() )
        return static_cast<int>(std::distance(holdings.begin(), it));

    return DOES_NOT_CONTAIN;
}

void HistoricalMarketData::addEquity(const std::string& ticker, const int step_unit, const int step_length)
{
    HistoricalEquityData hist(ticker, step_unit, step_length);

    hist_equities.push_back(hist);
}

void HistoricalMarketData::appendDataTo(const std::string& ticker, const EquitySnapshot& eq)
{
    int index = containsTicker(ticker);

    if( index == DOES_NOT_CONTAIN )
        throw std::runtime_error("Ticker not found in Historical Market");

    hist_equities[index].append_data(eq);
}

void HistoricalMarketData::appendDataTo(const std::string& ticker, const LiveEquity& leq)
{
    int index = containsTicker(ticker);

    if( index == DOES_NOT_CONTAIN )
        throw std::runtime_error("Ticker not found in Historical Market");

    hist_equities[index].append_data(leq);
}

void HistoricalMarketData::print(const int print_type) const
{

    std::vector<std::string> tickers = getTickers();

    std::vector<HistoricalEquityData> histories = getHistories();

    std::cout << "---------- Historical Market Data ----------" 
              << std::endl
              << "Equities Listed: "
              << std::endl;
    
    for( int i = 0; i < getSize(); i++ )
        std::cout << "- " << tickers[i] << std::endl;

    std::cout << "Histories: " << std::endl;

    for( int i = 0; i < getSize(); i++ )
        histories[i].print();

    std::cout << std::endl;
}

} // end namespace