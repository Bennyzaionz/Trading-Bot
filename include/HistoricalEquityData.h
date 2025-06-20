/*
This files defines the HistoricalMarketData class which contains data about a stock over time
It contains a vector of MarketSnapshots defined in Market_Snapshot.h
*/

#ifndef HISTORICAL_EQUITY_DATA_H
#define HISTORICAL_EQUITY_DATA_H

#include <string>
#include <vector>

#include "EquitySnapshot.h"
#include "LiveEquity.h"

namespace AlgoTrading
{

enum StepSizeUnit{ SECS, MINS, HOURS, DAYS, WEEKS, MONTHS };

const int NOT_CONTAINED = -1;

class HistoricalEquityData
{
    private:

        const std::string ticker;
        std::vector<EquitySnapshot> data;
        const int step_unit; // enum
        const int step_length;

        /*---------- SETTERS ----------*/

        void setData(std::vector<EquitySnapshot> ms) { data = ms; }

        /*---------- DATETIME HANDLER ----------*/

        int countDate(const DateTime& datetime_) const; // returns the number of times a date is present in list of datetimes
        DateTime datetimeHandler(const DateTime& datetime_) const;

    public:

    HistoricalEquityData(const std::string& ticker_,
                         const int step_unit_ = DAYS, 
                         const int step_length_ = 1);

    /*---------- GETTERS ----------*/

    std::string getTicker() const { return ticker; }

    std::vector<EquitySnapshot> getData() const { return data; }
    std::vector<DateTime> getDatetimes() const;
    std::vector<double> getHistoricalPrices(const int price_type) const;
    std::vector<int> getHistoricalVolume() const;

    int getStepUnit() const { return step_unit; }
    int getStepLength() const { return step_length; }
    int getSize() const { return data.size(); } // returns the size of the data vector (number of Equity snapshots)
    EquitySnapshot getSnapshotAt(std::string& datetime_) const;

    /*---------- APPENDING DATA ----------*/

    void append_data(const EquitySnapshot& eq); // { data.push_back(eq); }
    void append_data(const LiveEquity& leq); // { data.push_back(leq.getCurrentSnapshot()); }

    
    /*---------- PRINT HELPER ----------*/
    
    void print(const int print_type = BID_ASK) const;

    /*---------- CHECK CONTENTS ----------*/

    int containsDatetime(const std::string& datetime_) const;
    
};

} // namespace

#endif // HISTORICAL_EQUITY_DATA_H