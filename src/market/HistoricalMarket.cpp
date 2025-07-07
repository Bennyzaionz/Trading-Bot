/**
 * @file    HistoricalMarket.cpp
 * @brief   Defines the HistoricalMarket class functionality.
 *
 * HistoricalMarket is a collection of HistoricalEquity objects
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
#include "market/HistoricalMarket.h"

namespace AlgoTrading
{
 
HistoricalMarket::HistoricalMarket():
hist_equities{}, step_unit(StepSizeUnit::DAYS), step_length(1) {}

HistoricalMarket::HistoricalMarket(const StepSizeUnit step_unit_, const int step_length_):
hist_equities{}, step_unit(step_unit_), step_length(step_length_) {}

// HistoricalMarket::HistoricalMarket(std::vector<std::string> tickers):
// {

// }

std::vector<std::string> HistoricalMarket::getTickers() const
{
    
    // std::unordered_map<std::string, HistoricalEquity> histories = getHistories();

    std::vector<std::string> tickers;
    tickers.reserve(getSize());

    for( const auto& [ticker, hist] : hist_equities )
        tickers.push_back(ticker);

    return tickers;
}

HistoricalEquity HistoricalMarket::getHistory(const std::string& ticker) const
{
    auto it = hist_equities.find(ticker);
        if (it == hist_equities.end()) {
            throw std::runtime_error("Ticker '" + ticker + "' not found in HistoricalMarket");
        }
        return it->second;
}

bool HistoricalMarket::containsTicker(const std::string& ticker) const
{
    if (hist_equities.find(ticker) != hist_equities.end())
        return true;
    
    return false;
}

void HistoricalMarket::addEquity(const std::string& ticker, const bool verbose)
{

    // uses the constructor of HistoricalEquity with arg ticker 
    auto [it, inserted] = hist_equities.try_emplace(ticker, ticker);

    if( verbose )
    {
        if (inserted)
            std::cout << std::endl << "Equity " << ticker << " added to Historical Market" << std::endl;
        else
            std::cout << "Equity " << ticker << "FAILED to add to Historical Market" << std::endl;
    }


}

// void HistoricalMarket::appendDataTo(const std::string& ticker, const EquitySnapshot& eq)
// {
//     int index = containsTicker(ticker);

//     if( index == DOES_NOT_CONTAIN )
//         throw std::runtime_error("Ticker not found in Historical Market");

//     hist_equities[index].append_data(eq);
// }

void HistoricalMarket::appendData(const LiveEquity& leq)
{
    // int index = containsTicker(leq.getTicker());

    // const std::string ticker = leq.getTicker();

    // if( !containsTicker(ticker) )
    // {
    //     // throw std::runtime_error("Ticker not found in Historical Market");
    //     // add equity to historical market

    //     addEquity(ticker);
    // }

    // hist_equities[ticker].appendData(leq);

    const std::string& ticker = leq.getTicker();

    auto [it, inserted] = hist_equities.try_emplace(ticker, ticker);
    it->second.appendData(leq);
}


void HistoricalMarket::print(const PrintType print_type) const
{

    // std::vector<std::string> tickers = getTickers();

    std::unordered_map<std::string, HistoricalEquity> histories = getHistories();

    std::cout << "---------- Historical Market Data ----------" 
              << std::endl
              << "Equities Listed: "
              << std::endl;
    
    for( const auto& [ticker, _] : histories )
        std::cout << "- " << ticker << std::endl;

    std::cout << "Histories: " << std::endl;

    for( const auto& [_, history] : histories )
    {    
        history.print(print_type);
        // std::cout << std::endl;
    }

    std::cout << std::endl;
}

void HistoricalMarket::updateHistoricalMarket(const LiveMarket& lm)
{

    std::vector<std::shared_ptr<LiveEquity>> live_eq_ptrs = lm.getEquitiesPtrVector();

    std::string current_ticker;

    for( int i = 0; i < lm.getSize(); i++ )
    {
        current_ticker = live_eq_ptrs[i] -> getTicker();

        // if historical market does not contain the equity
        if( !containsTicker(current_ticker) )
        {
            addEquity(current_ticker);

            appendData(*live_eq_ptrs[i]);
        }

        // historical market does contain the equity
        else
        {
            DateTime lm_dt = live_eq_ptrs[i] -> getDatetime();

            DateTime hm_dt = getHistory(current_ticker).getMostRecentDateTime();

            // live market datetime is newer than the most recent historical data
            if( lm_dt > hm_dt )
            {
                appendData(*live_eq_ptrs[i]);
            }
        }
    }
}

} // end namespace