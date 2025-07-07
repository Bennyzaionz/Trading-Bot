#include "risk/RiskManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace AlgoTrading {

// Constructor definition
RiskManager::RiskManager(double max_risk_,
                         double max_position_,
                         int max_trades_,
                         double min_rr_,
                         double k_sl_)
    : max_risk_per_trade(max_risk_),
      max_position_value(max_position_),
      max_trades_per_day(max_trades_),
      min_risk_reward_ratio(min_rr_),
      k_sl(k_sl_) {}

// Risk check logic
// bool RiskManager::is_trade_allowed(double portfolio_value,
//                                    int shares,
//                                    double entry_price,
//                                    double stop_loss_price,
//                                    double take_profit_price) const
// {

//     double position_value = shares * entry_price;
//     if (position_value > max_position_value) {
//         std::cout << "[RiskManager] Rejected: position value exceeds limit.\n";
//         return false;
//     }

//     double risk_per_share = entry_price - stop_loss_price;
//     if (risk_per_share <= 0) {
//         std::cout << "[RiskManager] Rejected: invalid stop loss.\n";
//         return false;
//     }

//     double total_risk = risk_per_share * shares;
//     if (total_risk > max_risk_per_trade * portfolio_value) {
//         std::cout << "[RiskManager] Rejected: risk exceeds threshold.\n";
//         return false;
//     }

//     double reward_per_share = take_profit_price - entry_price;
//     if (reward_per_share <= 0) {
//         std::cout << "[RiskManager] Rejected: invalid take profit.\n";
//         return false;
//     }

//     double rr_ratio = reward_per_share / risk_per_share;
//     if (rr_ratio < min_risk_reward_ratio) {
//         std::cout << "[RiskManager] Rejected: reward/risk ratio too low (" << rr_ratio << ").\n";
//         return false;
//     }

//     return true;
// }

double RiskManager::trueRange(double high, double low, double prev_close) const
{
    return std::max(high - low,
                    std::max(std::abs(high - prev_close),
                             std::abs(low - prev_close))
                    );
}

double RiskManager::averageTrueRange(std::vector<EquitySnapshot> daily_hist, int num_days) const
{
    int num_tr = 0;
    double total_tr = 0;

    for( int i = 0; i < daily_hist.size(); i++ )
        total_tr += trueRange(daily_hist[i].getHigh(), daily_hist[i].getLow(), daily_hist[i].getLast());

    return total_tr / num_days;
}

std::pair<double, double> RiskManager::computeStops(double entry_price, double atr, bool is_long_pos) const
{

    double stop_loss, take_profit;

    if( is_long_pos )
    {
        stop_loss = entry_price - getKSL() * atr;
        take_profit = entry_price + getKTP() * atr;
    }

    else // short position
    {
        stop_loss = entry_price + getKSL() * atr;
        take_profit = entry_price - getKTP() * atr;       
    }

    return std::make_pair(stop_loss, take_profit);
}

int RiskManager::maxPositionSize(double portfolio_value, 
                                 double entry_price,
                                 HistoricalEquity& hist,
                                 int num_days) const
{

    std::vector<EquitySnapshot> daily_hist = hist.getDailySnapshots();

    // do not allow trades if there is insufficient historical data
    if( daily_hist.size() < num_days )
        return 0;

    double max_risk = portfolio_value * max_risk_per_trade;

    double ATR = 0;

    return 0;
}

} // namespace AlgoTrading
