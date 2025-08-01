/**
 * @file    HistoricalMarket.h
 * @brief   Defines the HistoricalMarket class.
 *
 * HistoricalMarket is a collection of HistoricalEquity objects
 * and is intended to be used to collect the history of many stocks.
 * Ideally in the future the HistoricalMarket class can interface with a database.
 * 
 * @author  Benny Zaionz
 * @date    2025-06-20
 * @version 1.0
 */

 #ifndef HISTORICAL_MARKET_DATA_H
 #define HISTORICAL_MARKET_DATA_H

 #include "market/HistoricalEquity.h"
 #include "market/LiveMarket.h"

 #include <vector>
 #include <unordered_map>

namespace AlgoTrading
{

enum class StepSizeUnit{ SECS, MINS, HOURS, DAYS, WEEKS, MONTHS };

class HistoricalMarket
{

    private:

        // std::vector<HistoricalEquity> hist_equities;
        std::unordered_map<std::string, HistoricalEquity> hist_equities;
        const StepSizeUnit step_unit; // enum, also used for risk manager to 
        const int step_length;

    public:

        /*---------- CONSTRUCTOR ----------*/

        HistoricalMarket();

        HistoricalMarket(const StepSizeUnit step_unit_, const int step_length_);

        // HistoricalMarketData(std::vector<std::string> tickers);

        /*---------- GETTERS ----------*/

        StepSizeUnit getStepUnit() const { return step_unit; }
        int getStepLength() const { return step_length; }
        // std::vector<HistoricalEquity> getHistories() const { return hist_equities; }
        std::unordered_map<std::string, HistoricalEquity> getHistories() const { return hist_equities; }
        std::pair < std::vector <std::string>, std::vector <HistoricalEquity> > getHistoriesVector() const;
        size_t getSize() const { return hist_equities.size(); }

        std::vector<std::string> getTickers() const;

        HistoricalEquity getHistory(const std::string& ticker) const;
        bool containsTicker(const std::string& ticker = "") const;

        /*---------- GET LATEST AVAILABLE DATA ----------*/
        DateTime getLatestDateTime(const std::string& ticker) const;
        double getLatestBid(const std::string& ticker) const;
        double getLatestAsk(const std::string& ticker) const;
        double getLatestLast(const std::string& ticker) const;
        double getLatestOpen(const std::string& ticker) const;
        double getLatestClose(const std::string& ticker) const;
        double getLatestHigh(const std::string& ticker) const;
        double getLatestLow(const std::string& ticker) const;

        /*---------- CHECK STATE ----------*/
        bool isEmpty() const { return hist_equities.empty(); }

        /*---------- ADDING EQUITIES ----------*/

        void addEquity(const std::string& ticker, const bool verbose = false);
        void appendData(const LiveEquity& leq);

        // used to update the entire historical market at once with a fully updated live market
        // likely will only be useful for back testing since callback for live update makes it difficult to know when the full market is updated
        // perhaps there is something that can be done with multithreading (i.e only switch to historical write thread when full update is finished?)
        // for now use only for back testing
        void updateHistoricalMarket(const LiveMarket& lm); // uses live time to

        /*----------  PRINT HELPER ----------*/

        void print(const PrintType print_type = PrintType::BID_ASK) const;

        /*---------- SIMULATING DATA ----------*/

        void simulateData(std::vector <std::string> tickers, int num_days);

};

 }

 #endif