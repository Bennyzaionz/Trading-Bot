#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <vector>
#include "trading/OpenPosition.h"
#include "trading/LimitOrder.h"
#include "market/LiveMarket.h"

namespace AlgoTrading
{



enum class TradeStatus{ SUCCESSFUL_TRADE, 
                        INSUFFICIENT_FUNDS, 
                        INSUFFICIENT_SHARES, 
                        SELL_INDEX_OUT_OF_BOUNDS, 
                        TICKER_NOT_TRACKED,
                        ERROR };

const double COMMISSION_PER_SHARE = 0.005; // from IBKR pro trading license: https://www.interactivebrokers.com/en/pricing/commissions-stocks.php
const int MIN_COMMISSION = 1; // also from IBKR pro trading license, any commission price less than 1 will round up to 1

class Portfolio
{
    private:
        
        double cash;
        std::vector<OpenPosition> pos;
        std::vector<LimitOrder> orders;

        double getCommission(int quantity) const;

        // appends to pos vector
        void addNewPosition(const std::shared_ptr<LiveEquity> leq, 
                            const int num_shares,
                            const double purchase_price,
                            const double stop_loss,
                            const double take_profit,
                            const DateTime& open_dt); 
                       
        void removePosition(int index);                         
        void removeShares(const int index, const int num_shares);

        TradeStatus buyEquity(const std::shared_ptr<LiveEquity> leq,
                              const int num_shares_buy, 
                              const double price,
                              const double stop_loss,
                              const double take_profit, 
                              const DateTime& dt,
                              const bool verbose = false); // important to populate equity with data once the str is added
        
        TradeStatus sellEquity(const int index, 
                               const int num_shares_sell, 
                               const double price, 
                               const bool verbose = false);

    public:
        
        /*---------- CONSTRUCTOR ----------*/

        Portfolio(const double cash_); // only construct Portfolio with empty equities and num_shares to ensure that amounts line up

        /*---------- GETTERS ----------*/
        
        size_t getNumPositions() const { return pos.size(); }
        size_t getNumLimitOrders() const { return orders.size(); }

        std::vector<std::string> getHoldings() const;

        std::vector<LiveEquity> getLiveEquities() const;

        std::vector<OpenPosition> getOpenPositions() const { return pos; }

        std::vector<int> getNumShares() const;

        int getNumSharesOf(const std::string& ticker);

        double getCash() const { return cash; }

        double getValue() const;
        
        std::vector<LimitOrder> getLimitOrders() const { return orders; }

        /*---------- PRINT HELPER ---------*/

        void print(const PrintType print_type = PrintType::BID_ASK) const;
        void printPositions() const;
        void printLimitOrders() const;

        /*---------- CONTENTS ----------*/

        int ContainsOpenOrderWithTicker(const std::string ticker_ = "") const; // returns the index of the ticker if it exists, otherwise returns -1

        /*---------- BUYING AND SELLING ----------*/

        TradeStatus marketOrder(const OrderType order_type,
                                const std::shared_ptr<LiveEquity> leq,
                                const int num_shares,
                                const bool verbose = false);

        TradeStatus marketOrder(const OrderType order_type,
                                const std::string ticker,
                                const LiveMarket& lm,
                                const int num_shares,
                                const bool verbose = false);

        TradeStatus limitOrder(const OrderType order_type,
                               const std::string&ticker,
                               const int num_shares,
                               const int desired_price,
                               const DateTime& dt,
                               const DateTime& exp,
                               const bool verbose = false);

        TradeStatus limitOrder(const OrderType order_type,
                               const LiveEquity& leq,
                               const int num_shares,
                               const int desired_price,
                               const DateTime& dt_placed,
                               const DateTime& exp,
                               const bool verbose = false);

        /*---------- UPDATING ----------*/

        UpdateType updateLiveEquity(const std::string& ticker,
                                    const double last_,
                                    const double low_,
                                    const double high_,
                                    const double bid_,
                                    const double ask_,
                                    const int volume_,
                                    const DateTime& dt_);

        

};

} // end namespace

#endif