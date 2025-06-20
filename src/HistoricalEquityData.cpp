/*
This files defines the HistoricalMarketData class which contains data about a stock over time
It contains a vector of MarketSnapshots defined in Market_Snapshot.h
*/

// #include <string>
// #include <vector>
// #include <iostream>

// #include "Market_Snapshot.h"

#include <algorithm>
#include <iostream>
#include <format>

#include "HistoricalEquityData.h"

namespace AlgoTrading
{

/*---------- CONSTRUCTOR ----------*/

HistoricalEquityData::HistoricalEquityData(const std::string& ticker_,
                                           const int step_unit_, 
                                           const int step_length_):
ticker(ticker_), data{}, step_unit(step_unit_), step_length(step_length_) {}

/*---------- GETTERS ----------*/

std::vector<DateTime> HistoricalEquityData::getDatetimes() const
{

    std::vector<DateTime> datetimes = {};

    std::vector<EquitySnapshot> snapshots = getData();

    for( int i = 0; i < getSize(); i++ )
    {
        datetimes.push_back(snapshots[i].getDatetime());
    }

    return datetimes;
}

std::vector<int> HistoricalEquityData::getHistoricalVolume() const
{
    std::vector<int> volumes = {};

    std::vector<EquitySnapshot> snapshots = getData();

    for( int i = 0; i < getSize(); i++ )
    {
        volumes.push_back(snapshots[i].getVolume());
    }

    return volumes;
}

EquitySnapshot HistoricalEquityData::getSnapshotAt(std::string& datetime_) const
{

    /*
    returns default EquitySnapshot if it is not found, use carefully
    */
    int index = containsDatetime(datetime_);

    if( index != -1)
    {
        return getData()[index];
    }

    std::cout << "---------- WARNING ----------" << std::endl
              << "Snapshot at: " << datetime_ 
              << " was not found in historical record of "
              << getTicker()
              << std::endl
              << "Returning default EquitySnapshot" << std::endl
              << "-----------------------------" << std::endl;

    return EquitySnapshot();
}

std::vector<double> HistoricalEquityData::getHistoricalPrices(const int price_type) const
{
    std::vector<double> prices = {};

    std::vector<EquitySnapshot> snapshots = getData();

    for( int i = 0; i < getSize(); i++ )
    {
        prices.push_back(snapshots[i].getPrice(price_type));
    }

    return prices;
}

/*---------- PRINTER HELPER ---------*/

void HistoricalEquityData::print(const int print_type) const
{
    std::cout << "---------- " << ticker << " History ----------" << std::endl;

    for ( int i = 0; i < data.size(); i++ )
    {
        data[i].print(print_type);
    }

    std::cout << "----------------------------------" << std::endl;
}

/*---------- CHECK CONTENTS ----------*/

int HistoricalEquityData::containsDatetime(const std::string& datetime_) const
{

    /*
    returns index of datetime if it exists in data, otherwise return NOT_CONTAINED
    */

    std::vector<DateTime> datetimes = getDatetimes();

    auto it = std::find(datetimes.begin(), datetimes.end(), datetime_);

    if( it != datetimes.end() )
        return static_cast<int>(std::distance(datetimes.begin(), it));

    return NOT_CONTAINED;
}

int HistoricalEquityData::countDate(const DateTime& datetime_) const
{

    /*
    returns the number of times a date shows up in the existing list
    of datetimes. This will be useful for using snapshots with
    granularity greater than 1 day (ex: 10 mins), since twsapi
    only gives the date "YYYYMMDD" and inference is required for the 
    time "HH:MM"
    */
    
    // std::string date_ = datetime_.substr(0, 4);

    std::vector<DateTime> hist_datetimes = getDatetimes();

    int count = 0;

    for( int i = 0; i < getSize(); i++ )
    {
        if( hist_datetimes[i].sameDateAs(datetime_) )
            count++;
    }

    return count;
}

void HistoricalEquityData::append_data(const EquitySnapshot& eq)
{

    EquitySnapshot modified = eq;

    DateTime corrected_dt = datetimeHandler(modified.getDatetime());

    modified.setDatetime(corrected_dt);

    data.push_back(std::move(modified));
}

void HistoricalEquityData::append_data(const LiveEquity& leq)
{
    append_data(leq.getCurrentSnapshot());
}

DateTime HistoricalEquityData::datetimeHandler(const DateTime& datetime_) const
{

    const int numInstances = countDate(datetime_);

    if( step_unit == SECS )
    {
        return datetime_ + numInstances*step_length;
    }

    else if( step_unit == MINS )
    {
        return datetime_ + numInstances*step_length*60;
    }

    else if( step_unit == HOURS )
    {
        return datetime_ + numInstances*step_length*3600;
    }

    return datetime_;
}

} // namespace