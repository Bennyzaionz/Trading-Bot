#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H

#include "market/HistoricalMarket.h"
#include "trading/Portfolio.h"
#include <utility>

namespace AlgoTrading {

class RiskManager // base risk manager, can be used out of the box, intended to be inhereted for more complex risk management strategies
{ 
    
    protected:

        double max_risk_per_trade;
        double max_position_value;
        int max_trades_per_day;
        double min_risk_reward_ratio;

        double k_sl; // stop loss const, take profit const = rr*k_sl

        double trueRange(double high, double low, double prev_close) const;
        double averageTrueRange(std::vector<EquitySnapshot> daily_hist, int num_days) const;

        // ---------- HELPER FUNCTIONS FOR COMPUTING PORTFOLIO VARIANCE ----------
        std::vector <double> daily_log_returns(const HistoricalEquity& heq, const int num_days) const;
        std::vector < std::vector <double> > compute_log_return_matrix(const std::vector<HistoricalEquity>& equities, int num_days) const; // matrix of log returns for equities over time
        std::vector < std::vector <double> > compute_covariance_matrix(const std::vector < std::vector <double> >& returns) const;
        double compute_portfolio_std_dev(const std::vector < std::vector <double> >& cov_matrix, const std::vector<double>& weights) const;
        std::vector <double> compute_portfolio_weights(const Portfolio& p, const HistoricalMarket& hm, const std::vector <std::string>& tickers) const;
         

        // write a function that computes the weights, then write a function to wrap everything for portfolio volatility where all thats needed is the portfolio and the historical market

    public:

        /*---------- CONSTRUCTOR ----------*/

        RiskManager(double max_risk = 0.02,
                    double max_position = 20000.0,
                    int max_trades = 10,
                    double min_rr = 2.0,
                    double k_sl_ = 1.5);

        /*---------- GETTERS ----------*/

        double getMaxRiskPerTrade() const { return max_risk_per_trade; }
        double getMaxPositionValue() const { return max_position_value; }
        int getMaxTradesPerDay() const { return max_trades_per_day; }
        double getMinRiskRewardRatio() const { return min_risk_reward_ratio; }
        double getKSL() const { return k_sl; }
        double getKTP() const { return k_sl * min_risk_reward_ratio; }

        /*---------- SETTERS ----------*/

        void setMaxRiskPerTrade(double risk) { max_risk_per_trade = risk; }
        void setMaxPositionValue(double value) { max_position_value = value; }
        void setMaxTradePerDay(int max_trades) { max_trades_per_day = max_trades; }
        void setMinRiskRewardRatio(double ratio) { min_risk_reward_ratio = ratio; }

        /*---------- RISK VALIDATION ----------*/

        std::pair<double, double> computeStops(double entry_price, double atr, bool is_long_pos) const;

        int maxPositionSize(double portfolio_value, 
                            double entry_price,
                            HistoricalEquity& hist,
                            int num_days = 10) const;

        bool is_trade_allowed(double portfolio_value,
                            int shares,
                            double entry_price,
                            double stop_loss_price,
                            double take_profit_price) const;

        virtual double compute_equity_risk(const std::string& ticker, const HistoricalMarket& hm) const; // for 1 share of a single equity
        virtual double compute_equity_risk(const HistoricalEquity& heq) const;
        
        virtual double compute_portfolio_risk(const Portfolio& p, const HistoricalMarket& hm) const;

};

} // namespace AlgoTrading

#endif // RISK_MANAGER_H
