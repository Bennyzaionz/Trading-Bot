#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H

#include "market/HistoricalEquity.h"
#include <utility>

namespace AlgoTrading {

class RiskManager {
    
    private:

        double max_risk_per_trade;
        double max_position_value;
        int max_trades_per_day;
        double min_risk_reward_ratio;

        double k_sl; // stop loss const, take profit const = rr*k_sl

        double trueRange(double high, double low, double prev_close) const;
        double averageTrueRange(std::vector<EquitySnapshot> daily_hist, int num_days) const;

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


};

} // namespace AlgoTrading

#endif // RISK_MANAGER_H
