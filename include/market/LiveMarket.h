#ifndef LIVE_MARKET_H
#define LIVE_MARKET_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

#include "market/LiveEquity.h"

namespace AlgoTrading {

class LiveMarket {
private:
    std::unordered_map<std::string, std::shared_ptr<LiveEquity>> equities;

public:

    size_t getSize() const { return equities.size(); }

    bool containsTicker(const std::string& ticker) const;

    // Add a new equity (only if it doesn't already exist)
    void addEquity(const std::string& ticker);

    // Get equity by ticker
    std::shared_ptr<LiveEquity> getEquity(const std::string& ticker) const;

    // Get all equities
    std::vector<std::shared_ptr<LiveEquity>> getEquitiesPtrVector() const;
    std::unordered_map<std::string, std::shared_ptr<LiveEquity>> getEquitiesPtrMap() const { return equities; }

    // For live mode: update equity values from TWS API callbacks
    void updateLiveEquity(const std::string& ticker,
                           const double last,
                           const double low,
                           const double high,
                           const double bid,
                           const double ask,
                           const int volume,
                           const DateTime& dt);

    /*---------- PRINT HELPERS ----------*/
    void print(const PrintType print_type = PrintType::BID_ASK) const;
};

} // namespace AlgoTrading

#endif
