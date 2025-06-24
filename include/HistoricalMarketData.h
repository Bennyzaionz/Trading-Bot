/**
 * @file    HistoricalMarketData.h
 * @brief   Defines the HistoricalMarketData class.
 *
 * HistoricalMarketData is a collection of HistoricalEquityData objects
 * and is intended to be used to collect the history of many stocks.
 * 
 *
 * @author  Benny Zaionz
 * @date    2025-06-20
 * @version 1.0
 */

 #ifndef HISTORICAL_MARKET_DATA_H
 #define HISTORICAL_MARKET_DATA_H

 #include "HistoricalEquityData.h"

 #include <vector>

 namespace AlgoTrading
 {

class HistoricalMarketData
{

    private:

        std::vector<HistoricalEquityData> hist_equities;

    public:

        /*---------- CONSTRUCTOR ----------*/

        HistoricalMarketData();

        // HistoricalMarketData(std::vector<std::string> tickers);

        /*---------- GETTERS ----------*/

        std::vector<std::string> getTickers() const;
        std::vector<HistoricalEquityData> getHistories() const { return hist_equities; }
        HistoricalEquityData getHistory(const std::string& ticker) const;
        // int getIndex(const std::string& ticker, const DateTime& dt);
        int getSize() const { return hist_equities.size(); }
        int containsTicker(const std::string ticker = "") const; // returns the index of the ticker if it exists, otherwise returns -1

        /*---------- ADDING EQUITIES ----------*/

        void addEquity(const std::string& ticker, const int step_unit, const int step_length);
        void appendDataTo(const std::string& ticker, const LiveEquity& leq);
        void appendDataTo(const std::string& ticker, const EquitySnapshot& eq);

        /*----------  PRINT HELPER ----------*/

        void print(const int print_type = BID_ASK) const;

};

 }

 #endif