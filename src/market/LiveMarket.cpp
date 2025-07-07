#include "market/LiveMarket.h"

namespace AlgoTrading
{

bool LiveMarket::containsTicker(const std::string& ticker) const
{
    return equities.find(ticker) != equities.end();
}

void LiveMarket::addEquity(const std::string& ticker)
{
    if( !containsTicker(ticker) )
        equities[ticker] = std::make_shared<LiveEquity>(ticker);
}

std::shared_ptr<LiveEquity> LiveMarket::getEquity(const std::string& ticker) const
{
    auto it = equities.find(ticker);

    if( it != equities.end() )
        return it -> second;
    
    return nullptr;
}

std::vector<std::shared_ptr<LiveEquity>> LiveMarket::getEquitiesPtrVector() const
{
    std::vector<std::shared_ptr<LiveEquity>> leqs = {};

    leqs.reserve(getSize());

    for( const auto& [_, equityPtr] : equities)
        leqs.push_back(equityPtr);

    return leqs;
}

void LiveMarket::updateLiveEquity(const std::string& ticker,
                                  const double last,
                                  const double low,
                                  const double high,
                                  const double bid,
                                  const double ask,
                                  const int volume,
                                  const DateTime& dt)
{
    auto it = equities.find(ticker);
    if( containsTicker(ticker) )
        it -> second -> updateEquitySnapshot(last, low, high, bid, ask, volume, dt);
}

void LiveMarket::print(const PrintType print_type) const
{
    std::cout << std::endl << "---------- Live Market ----------" << std::endl;

    for( const auto& [_, equityPtr] : equities)
        equityPtr -> print(print_type); 
        
    std::cout << std::endl << "---------------------------------" << std::endl;
}

}