/*
This files defines the HistoricalMarketData class which contains data about a stock over time
It contains a vector of MarketSnapshots defined in Market_Snapshot.h
*/

#ifndef HISTORICAL_EQUITY_DATA_H
#define HISTORICAL_EQUITY_DATA_H

#include <string>
#include <vector>
#include <iostream>

#include "EquitySnapshot.h"

namespace AlgoTrading
{

class HistoricalEquityData
{
    private:

        const std::string ticker;
        std::vector<EquitySnapshot> data;
        const std::string timescale_unit;
        const int timescale_length;

    public:

    HistoricalEquityData
    (
        const std::string ticker_ = "",
        const std::string timescale_unit_ = "D", 
        const int timescale_length_ = 1
    );

    HistoricalEquityData
    (
        const std::string ticker_ = "",
        const std::vector<EquitySnapshot>& vector_data = {}, 
        const std::string timescale_unit_ = "D", 
        const int timescale_length_ = 1
    );

    /*---------- GETTERS ----------*/

    std::string getTicker() const { return ticker; }
    std::vector<EquitySnapshot> getData() const { return data; }
    std::string getTimescaleUnit() const { return timescale_unit; }
    int getTimescaleLength() const { return timescale_length; }

    /*---------- SETTERS ----------*/

    void setData(std::vector<EquitySnapshot> ms) { data = ms; }

    /*---------- APPENDING DATA ----------*/

    void append_data(const EquitySnapshot ms) { data.push_back(ms); } // append using EquitySnapshot item
    
    void append_data( const std::string datetime_ = "", 
                     double open_ = -1, double close_ = -1, double low_ = -1, double high_ = -1,
                     double avg_bid_ = -1, double avg_ask_ = -1, double min_bid_ = -1, double max_ask_ = -1,
                     int volume_ = 0) // append using raw data
    {
        EquitySnapshot ms(datetime_, open_, close_, low_, high_, avg_bid_, avg_ask_, min_bid_, max_ask_, volume_);
        data.push_back(ms);
    }

    /*---------- SIZE ----------*/

    int getSize() { return data.size(); } // returns the size of the data vector (number of Equity snapshots)

    /*---------- PRINT HELPER ----------*/
    
    void print(const std::string print_type);

    // MarketSnapshot get_MarketSnapshot_from(std::str datetime)

};

} // namespace

#endif // HISTORICAL_EQUITY_DATA_H