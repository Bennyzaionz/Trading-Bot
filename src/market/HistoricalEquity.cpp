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
#include <random>

#include "market/HistoricalEquity.h"

namespace AlgoTrading
{

/*---------- CONSTRUCTOR ----------*/

HistoricalEquity::HistoricalEquity(const std::string& ticker_):
ticker(ticker_), data{} {}

/*---------- GETTERS ----------*/

std::vector<DateTime> HistoricalEquity::getDatetimes() const
{

    std::vector<DateTime> datetimes = {};

    std::vector<EquitySnapshot> snapshots = getData();

    for( int i = 0; i < getSize(); i++ )
    {
        datetimes.push_back(snapshots[i].getDateTime());
    }

    return datetimes;
}

std::vector<int> HistoricalEquity::getHistoricalVolume() const
{
    std::vector<int> volumes = {};

    std::vector<EquitySnapshot> snapshots = getData();

    for( int i = 0; i < getSize(); i++ )
    {
        volumes.push_back(snapshots[i].getVolume());
    }

    return volumes;
}

EquitySnapshot HistoricalEquity::getSnapshotAt(std::string& datetime_) const
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

std::vector<double> HistoricalEquity::getHistoricalPrices(const PriceType price_type) const
{
    std::vector<double> prices = {};

    std::vector<EquitySnapshot> snapshots = getData();

    for( int i = 0; i < getSize(); i++ )
    {
        prices.push_back(snapshots[i].getPrice(price_type));
    }

    return prices;
}

int HistoricalEquity::getNumDays() const
{
    int num_days = 0;
    int curr_day = 0;
    int prev_day = 0;

    std::vector<DateTime> dts = getDatetimes();

    for( int i = 0; i < getSize(); i++)
    {
        curr_day = dts[i].getDay();

        if( curr_day != prev_day)
            num_days++;

        prev_day = curr_day;
    }

    return num_days;
}

std::vector<EquitySnapshot> HistoricalEquity::getDailySnapshots() const
{

    // if( getStepUnit() == DAYS && getStepLength() == 1)
    //     return getData();
    
    std::vector<EquitySnapshot> daily_snaps = {};

    std::vector<EquitySnapshot> snaps = getData();

    std::vector<DateTime> dts = getDatetimes();

    EquitySnapshot snap;

    int num_days = 0;
    int curr_day = dts[0].getDay();
    int next_day = 0;

    for( int i = 0; i < getSize() - 1; i++) // iterate through next date
    {
        next_day = dts[i + 1].getDay();

        // handle combining sub-day data
        if( snaps[i].getHigh() > snap.getHigh() )
            snap.setHigh(snaps[i].getHigh());            
            
        if( snaps[i].getLow() < snap.getLow() )
            snap.setLow(snaps[i].getLow());

        if( next_day != curr_day ) // last snapshot of day
        {
            snap.setLast(snaps[i].getLast());
            snap.setOpen(snaps[i].getOpen());
            snap.setClose(snaps[i].getClose());

            daily_snaps.push_back(snap);

            snap.setHigh(-1);
            snap.setLow(100000);
            snap.setLast(-1);
        }

        curr_day = next_day;
    }

    return daily_snaps;
}

DateTime HistoricalEquity::getMostRecentDateTime() const 
{
    std::vector<DateTime> dts = getDatetimes();

    if (dts.empty()) {
        throw std::runtime_error("No datetimes available");
    }
    return dts.back();
}


/*---------- PRINTER HELPER ---------*/

void HistoricalEquity::print(const PrintType print_type) const
{
    std::cout << "---------- " << ticker << " History ----------" << std::endl;

    for ( int i = 0; i < getSize(); i++ )
    {
        data[i].print(print_type);
        std::cout << std::endl;
    }

    std::cout << "----------------------------------" << std::endl;
}

/*---------- CHECK CONTENTS ----------*/

int HistoricalEquity::containsDatetime(const std::string& datetime_) const
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

int HistoricalEquity::countDate(const DateTime& datetime_) const
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
        if( hist_datetimes[i] == datetime_ )
            count++;
    }

    return count;
}

void HistoricalEquity::appendData(const LiveEquity& leq)
{
    // append_data(leq.getCurrentSnapshot());

    data.push_back(leq.getCurrentSnapshot());
}

void HistoricalEquity::simulateData(int num_days,
                                    double start_price,
                                    const DateTime& start_date,
                                    double volatility,
                                    double drift,
                                    double bid_ask_spread,
                                    double volume_mean,
                                    double volume_stddev)
{
    data.clear();
    data.reserve(num_days);

    LiveEquity leq;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> z(0.0, 1.0);
    std::normal_distribution<> vol_dist(volume_mean, volume_stddev);
    std::normal_distribution<> spread_noise(0.0, bid_ask_spread * 0.25);

    double price = start_price;
    DateTime current_date = start_date;

    for (int i = 0; i < num_days; ++i) {
        double return_pct = drift + volatility * z(gen);
        double close = price * std::exp(return_pct);
        double last = close;

        double range = close * 0.01;
        double open = price;
        double high = std::max(open, close) + std::abs(range * z(gen));
        double low = std::min(open, close) - std::abs(range * z(gen));

        double spread = bid_ask_spread + spread_noise(gen);
        double bid = close - spread / 2.0;
        double ask = close + spread / 2.0;

        double volume = std::max(0.0, vol_dist(gen));

        // snapshots.push_back({open, high, low, close, bid, ask, volume, current_date});

        leq.updateEquitySnapshot(open, close, last, low, high, bid, ask, (int)volume, current_date);

        appendData(leq);

        price = close;
        current_date = current_date + 86400; // add 1 day (in seconds)
    }
}

} // namespace