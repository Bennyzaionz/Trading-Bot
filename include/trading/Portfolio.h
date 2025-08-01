#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <vector>
#include "trading/OpenPosition.h"
#include "trading/LimitOrder.h"
// #include "market/LiveMarket.h"
#include "market/HistoricalMarket.h"
// #include "risk/RiskManager.h"



namespace AlgoTrading
{

class RiskManager; // forward declare to avoid circular inclusion with Portfolio.h

enum class TradeStatus{ SUCCESSFUL_TRADE, 
                        INSUFFICIENT_FUNDS, 
                        INSUFFICIENT_SHARES, 
                        PARTIAL_FILL,
                        SELL_INDEX_OUT_OF_BOUNDS, 
                        TICKER_NOT_TRACKED,
                        ERROR };

const double COMMISSION_PER_SHARE = 0.005; // from IBKR pro trading license: https://www.interactivebrokers.com/en/pricing/commissions-stocks.php
const int MIN_COMMISSION = 1; // also from IBKR pro trading license, any commission price less than 1 will round up to 1

class Portfolio
{
    private:
        
        // portfolio values that need to be tracked
        double cash;
        std::vector<OpenPosition> pos;
        std::vector<LimitOrder> orders;

        // references for classes that are used frequently
        const HistoricalMarket& hm;

        double getCommission(int quantity) const;

        // appends to pos vector
        // void addNewPosition(const std::shared_ptr<LiveEquity> leq, 
        //                     const int num_shares,
        //                     const double purchase_price,
        //                     const double stop_loss,
        //                     const double take_profit,
        //                     const DateTime& open_dt); 

        void addNewPosition(const std::string& ticker,
                            const int num_shares,
                            const double purchase_price,
                            const RiskManager& rm);

        void removePosition(const int index);

        void removeShares(const int index, const int num_shares);

        // TradeStatus buyEquity(const std::shared_ptr<LiveEquity> leq,
        //                       const int num_shares_buy, 
        //                       const double price,
        //                       const RiskManager& rm, 
        //                       const DateTime& dt,
        //                       const bool verbose = false); // important to populate equity with data once the str is added

        TradeStatus buyEquity(const std::string& ticker,
                              const int num_shares_buy,
                              const double purchase_price,
                              const RiskManager& rm,
                              const bool verbose); // used to compute stop loss and take profit which is required for every position
        
        TradeStatus sellEquity(const int index, 
                               const int num_shares_sell, 
                               const double price, 
                               const bool verbose = false);

        TradeStatus attemptSellNumShares(const std::string& ticker, 
                                        const int num_shares_attempt_sell, 
                                        const double price, 
                                        const bool verbose);

        int getFillVolume(const OrderType order_type,
                                const double order_price,
                                const double live_low,
                                const double live_high,  
                                const int live_volume);

        double getFillPrice(const OrderType order_type, 
                            const double live_low, 
                            const double live_high, 
                            const double order_price);

        void removeLimitOrder(const int index);

    public:
        
        /*---------- CONSTRUCTOR ----------*/

        Portfolio(const HistoricalMarket& hm_, const double cash_);

        /*---------- GETTERS ----------*/
        
        size_t getNumPositions() const { return pos.size(); }
        size_t getNumLimitOrders() const { return orders.size(); }

        std::vector<std::string> getHoldings() const;
        std::vector <std::string> getUniqueHoldings() const;

        // std::vector<LiveEquity> getLiveEquities() const;

        std::vector<OpenPosition> getOpenPositions() const { return pos; }

        std::vector<int> getNumShares() const;

        std::vector<size_t> getOpenPositionIndeces(const std::string& ticker) const;

        std::vector<LimitOrder> getLimitOrders() const { return orders; }

        int getNumSharesOf(const std::string& ticker) const;

        double getCash() const { return cash; }

        double getCashValue() const; // gets sum of last traded prices

        double getValue() const; // gets sum of bid prices

        /*---------- PRINT HELPER ---------*/

        void print(const PrintType print_type = PrintType::BID_ASK) const;
        void printPositions() const;
        void printLimitOrders() const;

        /*---------- CONTENTS ----------*/

        int ContainsOpenOrderWithTicker(const std::string ticker_ = "") const; // returns the index of the ticker if it exists, otherwise returns -1

        /*---------- Orders ----------*/

        TradeStatus marketOrder(const OrderType order_type,
                                const std::string& ticker,
                                const int num_shares,
                                const RiskManager& rm,
                                const bool verbose);

        TradeStatus marketOrder(const std::string& ticker,
                                const int num_shares,
                                const RiskManager& rm,
                                const bool verbose);

        // rm already ensured that order can be purchased with available cash
        TradeStatus basketMarketOrder(const std::vector <std::pair <std::string, int> >& basket,
                                      const RiskManager& rm,
                                      const bool verbose);

        TradeStatus limitOrder(const OrderType order_type,
                               const std::string& ticker,
                               const int num_shares,
                               const int desired_price,
                               const DateTime& dt_order_placed,
                               const DateTime& dt_exp,
                               const bool verbose);



        void deleteExpiredLimitOrders(const DateTime& dt_curr, const bool verbose = false);

        void executeLimitOrders(const LiveMarket& lm, const RiskManager& rm, const bool verbose = false);

        // 
        void executeStopLossTakeProfit(const bool is_live, const LiveMarket&lm, const RiskManager& rm, const bool verbose = false);

        // only used for live trading since broker will store cash amount, and executing orders
        // TODO:
        void updatePortfolio();

};

} // end namespace

#endif