#ifndef OPEN_POSITION_H
#define OPEN_POSITION_H

#include "market/LiveEquity.h"
#include <memory>

namespace AlgoTrading {

class OpenPosition {

    private:
        std::shared_ptr<LiveEquity> live_equity;
        int shares;
        double entry_price;
        double stop_loss;
        double take_profit;
        DateTime open_dt;

    public:

        /*---------- CONSTRUCTOR ----------*/
        OpenPosition(const std::shared_ptr<LiveEquity>& live_equity_,
                    int shares_,
                    double entry_price_,
                    double stop_loss_,
                    double take_profit_,
                    const DateTime& open_dt_);

        OpenPosition(const std::string& ticker_,
                    int shares_,
                    double entry_price_,
                    double stop_loss_,
                    double take_profit_,
                    const DateTime& open_dt_);

        /*---------- GETTERS ----------*/

        const LiveEquity& getLiveEquity() const { return *live_equity; }
        int getShares() const { return shares; }
        double getEntryPrice() const { return entry_price; }
        double getStopLoss() const { return stop_loss; }
        double getTakeProfit() const { return take_profit; }
        double getMarketValue() const { return shares * getLiveEquity().getMarketValue(); }
        DateTime getOpenDT() const { return open_dt; }
        std::string getTicker() const { return getLiveEquity().getTicker(); }

        /*---------- SETTERS ----------*/

        void setShares(int shares_) { shares = shares_; }
        void setEntryPrice(double entry_price_) { entry_price = entry_price_; }
        void setStopLoss(double stop_loss_) { stop_loss = stop_loss_; }
        void setTakeProfit(double take_profit_) { take_profit = take_profit_; }

        /*---------- UPDATE LIVE DATA ----------*/

        void updateLiveEquity(const double last_,
                              const double low_,
                              const double high_,
                              const double bid_,
                              const double ask_,
                              const int volume_,
                              const DateTime& dt_);

        /*---------- CHANGING NUMBER OF SHARES ----------*/

        void subtractShares(const int num_shares) { setShares( getShares() - num_shares); }

        /*---------- STOP LOSS ----------*/

        bool hitStopLoss() const;
        bool hitTakeProfit() const;

        /*---------- PRINT HELPER ----------*/

        void print() const;
};

} // namespace AlgoTrading

#endif // OPEN_POSITION_H
