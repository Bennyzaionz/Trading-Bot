/*
This files defines the HistoricalMarketData class which contains data about a stock over time
It contains a vector of MarketSnapshots defined in Market_Snapshot.h
*/

// #include <string>
// #include <vector>
// #include <iostream>

// #include "Market_Snapshot.h"

#include "HistoricalEquityData.h"

namespace AlgoTrading
{

/*---------- CONSTRUCTOR ----------*/
HistoricalEquityData::HistoricalEquityData
(
    const std::string ticker_,
    const std::string timescale_unit_, 
    const int timescale_length_
):
ticker(ticker_), data{}, timescale_unit(timescale_unit_), timescale_length(timescale_length_) {}

HistoricalEquityData::HistoricalEquityData
(
    const std::string ticker_,
    const std::vector<EquitySnapshot>& vector_data, 
    const std::string timescale_unit_, 
    const int timescale_length_
):
ticker(ticker_), data(vector_data), timescale_unit(timescale_unit_), timescale_length(timescale_length_) {}

/*---------- PRINTER HELPER ---------*/

void HistoricalEquityData::print(const std::string print_type)
{
    std::cout << "---------- Ticker: " << ticker << " ----------" << std::endl;

    for ( int i = 0; i < data.size(); i++ )
    {
        data[i].print(print_type);
    }
}

    // MarketSnapshot get_MarketSnapshot_from(std::str datetime)

} // namespace