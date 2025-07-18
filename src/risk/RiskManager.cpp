#include "risk/RiskManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>

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

std::vector <double> RiskManager::daily_log_returns(const HistoricalEquity& heq, const int num_days) const
{
    std::vector <EquitySnapshot> snaps = heq.getDailySnapshots();

    if( snaps.size() < num_days + 1) // must account for i-1 for log returns
        return {-1.0};

    std::vector <double> log_returns;

    for( int i = snaps.size() - 1; i >= snaps.size() - num_days - 1; i--) // iterate backwards to get most recent days
    {
        log_returns.push_back( std::log( snaps[i].getClose() / snaps[i-1].getClose() ) );
    }

    return log_returns;
}

std::vector < std::vector <double> > RiskManager::compute_log_return_matrix(const std::vector<HistoricalEquity>& equities, const int num_days) const // rows = days, columns = equities
{
    int num_assets = equities.size();

    // declare matrix to store returns for each day for each asset
    std::vector < std::vector <double> > return_matrix(num_days, std::vector<double>(num_assets)); // rows = days, columns = equities

    // populate matrix with daily log returns
    for( int asset = 0; asset < num_assets; asset++ )
    {
        const std::vector<EquitySnapshot>& snaps = equities[asset].getDailySnapshots();

        std::vector <double> log_returns = daily_log_returns(equities[asset], num_days); // returns most recent day in position 0
        
        if( snaps.size() < num_days + 1)
        {
            throw std::runtime_error("Not enough data for one of the assets");
        }

        for( int day = 0; day < num_days; day++ )
        {
            return_matrix[day][asset] = log_returns[day];
        }
    }

    return return_matrix;
}

std::vector < std::vector <double> > RiskManager::compute_covariance_matrix(const std::vector < std::vector <double> >& returns) const
{
    int num_days = returns.size();
    int num_assets = returns[0].size();

    // compute mean for each column
    std::vector <double> mean_returns(num_assets, 0.0);

    for( const auto& row : returns )
        for( int i =0; i < num_assets; i++ )
            mean_returns[i] += row[i];
    for( double &m : mean_returns)
        m /= num_days;

    // compute covariance matrix
    std::vector< std::vector <double> > cov_matrix(num_assets, std::vector<double>(num_assets, 0.0));

    // use ++i for very slight performance improvement (no copy required)
    for (int i = 0; i < num_assets; ++i) // loop through assets
    {
        for (int j = i; j < num_assets; ++j) // only compute upper triangle (also loops through assets)
        {
            double cov = 0.0;
            for (int k = 0; k < num_days; ++k) // compute covariance between asset i and j across num_days
            {
                cov += (returns[k][i] - mean_returns[i]) * (returns[k][j] - mean_returns[j]); // contribution to covariance between asset i, and j, based on returns from day k
            }
            cov /= (num_days - 1);
            cov_matrix[i][j] = cov_matrix[j][i] = cov; // mirror upper triangle
        }
    }

    return cov_matrix;    
}

double RiskManager::compute_portfolio_std_dev(const std::vector < std::vector <double> >& cov_matrix, const std::vector<double>& weights) const
{
    int N = weights.size();
    double variance = 0.0;

    // wT * sigma * w
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            variance += weights[i] * weights[j] * cov_matrix[i][j];

    return std::sqrt(variance);
}

std::vector <double> RiskManager::compute_portfolio_weights(const Portfolio& p, const HistoricalMarket& hm, const std::vector <std::string>& tickers) const
{
    // std::vector <std::string> unique_tickers = p.getUniqueHoldings();
    int num_unique_holdings = tickers.size();
    
    std::vector <double> weights(num_unique_holdings);
    // weights.reserve(num_unique_holdings);

    for( int i = 0; i < num_unique_holdings; i++ )
    {
        // weights[i] = p.getNumSharesOf(tickers[i]) * lm.getEquity(tickers[i]) -> getLast();
        weights[i] = p.getNumSharesOf(tickers[i]) * hm.getHistory(tickers[i]).getLastTradePrice();
    }

    double portfolio_value = std::accumulate(weights.begin(), weights.end(), 0.0);

    for( int i = 0; i < num_unique_holdings; i++ )
    {
        weights[i] /= portfolio_value;
    }

    return weights;
}

double RiskManager::compute_portfolio_risk(const Portfolio& p, const HistoricalMarket& hm) const
{
    const int num_days = 20; // number of historical days to look at for returns

    // get vector of historical equities (and associated tickers)
    const std::pair < std::vector <std::string>, std::vector <HistoricalEquity> > heq_pair = hm.getHistoriesVector();

    const std::vector <std::string> tickers = heq_pair.first;
    const std::vector <HistoricalEquity> heq_vec = heq_pair.second;
    
    const std::vector < std::vector <double> > returns_matrix = compute_log_return_matrix(heq_vec, num_days);

    const std::vector < std::vector <double> > cov_matrix = compute_covariance_matrix(returns_matrix);

    const std::vector weights = compute_portfolio_weights(p, hm, tickers);

    return compute_portfolio_std_dev(cov_matrix, weights);
}

double RiskManager::compute_equity_risk(const std::string& ticker, const HistoricalMarket& hm) const 
{
    return compute_equity_risk(hm.getHistory(ticker));
}

double RiskManager::compute_equity_risk(const HistoricalEquity& heq) const 
{
    int num_days = 20; // number of days used for close to close volatility
    std::vector<EquitySnapshot> snaps = heq.getDailySnapshots();

    if( snaps.size() < num_days + 1) // must account for i-1 for log returns
        return -1;

    std::vector <double> log_returns = daily_log_returns(heq, num_days);

    double mean_returns = std::accumulate(log_returns.begin(), log_returns.end(), 0) / num_days;

    double diff_squared = 0;

    // compute standard deviation of log_returns
    for( int i = 0; i < num_days; i++)
    {
        diff_squared += (mean_returns - log_returns[i]) * (mean_returns - log_returns[i]);
    }

    return std::sqrt(diff_squared / (num_days - 1));
}

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
