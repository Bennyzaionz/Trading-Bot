/**
 * @file    HistoricalEquity.h
 * @brief   Defines the HistoricalEquityData class representing the price history of a single equity.
 *
 * This file contains the declaration of the HistoricalEquity class, which stores last, low, high, bid, and ask prices,
 * as well as volume for an equity at a specific point in time. 
 * It is used by the AlgoTrading system for both backtesting and live trading data processing.
 *
 * @author  Benny Zaionz
 * @date    2025-06-18
 * @version 1.0
 */

#ifndef HISTORICAL_EQUITY_DATA_H
#define HISTORICAL_EQUITY_DATA_H

#include <string>
#include <vector>

#include "market/LiveEquity.h"

namespace AlgoTrading
{

const int NOT_CONTAINED = -1;

class HistoricalEquity
{
    private:

        const std::string ticker;
        std::vector<EquitySnapshot> data;

        /*---------- SETTERS ----------*/

        void setData(std::vector<EquitySnapshot> ms) { data = ms; }

        /*---------- DATETIME FUNCTION ----------*/

        int countDate(const DateTime& datetime_) const; // returns the number of times a date is present in list of datetimes

    public:

    HistoricalEquity(const std::string& ticker_);

    /*---------- GETTERS ----------*/

    std::string getTicker() const { return ticker; }

    std::vector<EquitySnapshot> getData() const { return data; }
    std::vector<EquitySnapshot> getDailySnapshots() const; // returns daily snapshot values

    EquitySnapshot getSnapshotAt(std::string& datetime_) const;

    std::vector<DateTime> getDatetimes() const;

    DateTime getMostRecentDateTime() const;

    std::vector<double> getHistoricalPrices(const PriceType price_type) const;

    std::vector<int> getHistoricalVolume() const;

    size_t getSize() const { return data.size(); } // returns the size of the data vector (number of Equity snapshots)

    int getNumDays() const;


    /*---------- APPENDING DATA ----------*/

    void appendData(const LiveEquity& leq); 

    
    /*---------- PRINT HELPER ----------*/
    
    void print(const PrintType print_type = PrintType::BID_ASK) const;

    /*---------- CHECK CONTENTS ----------*/

    int containsDatetime(const std::string& datetime_) const;
    
};

} // namespace

#endif // HISTORICAL_EQUITY_DATA_H