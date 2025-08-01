#include "risk/RiskManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace AlgoTrading {

/*---------- CONSTRUCTORS ----------*/

RiskManager::RiskManager(const Portfolio& p_, const HistoricalMarket& hm_, double max_value_per_trade_, double max_portfolio_risk_) : 
                         max_value_per_trade(max_value_per_trade_), max_portfolio_risk(max_portfolio_risk_), p(p_), hm(hm_), tickers(hm_.getTickers()) {}

/*---------- SETTERS ----------*/

Eigen::VectorXd RiskManager::getDailyLogReturns(const HistoricalEquity& heq, const int num_days) const
{
    std::vector <EquitySnapshot> snaps = heq.getDailySnapshots();

    if( snaps.size() < num_days + 1) // must account for i-1 for log returns
        return Eigen::VectorXd(-1);

    Eigen::VectorXd log_returns(num_days);

    int snaps_index;
    int num_snaps = snaps.size();

    for( int i = 0; i < num_days; i++ )
    {
        snaps_index = num_snaps - 1 - i;

        // fill log_returns starting at 0 and increasing
        // index snaps from end -> 0 for num_days
        log_returns(i) = std::log( snaps[snaps_index].getClose() / snaps[snaps_index-1].getClose() );
    }

    return log_returns;
}

int RiskManager::getIndexOfTicker(const std::string& ticker) const
{
    auto it = std::find(tickers.begin(), tickers.end(), ticker);
    if( it != tickers.end() )
    {
        return std::distance(tickers.begin(), it);
    }
    else
        return -1;
}

/*---------- UPDATING RISK DATA ----------*/

// tickers vector is to keep consistent indexing of equities in matrices across helper functions, call hm.getTickers() in wrapper function and pass to all helper functions
Eigen::MatrixXd RiskManager::computeReturnsMatrix(const int num_days) const // rows = days, columns = equities
{

    int rows = num_days;

    int columns = tickers.size(); // number if equities in the historical market

    Eigen::MatrixXd returns_matrix(rows, columns);

    for( int i = 0; i < columns; i++ )
    {
        returns_matrix.col(i) = getDailyLogReturns(hm.getHistory(tickers[i]), num_days);
    }

    return returns_matrix;
}

Eigen::RowVectorXd RiskManager::computeExpectedReturns(const Eigen::MatrixXd& returns) const
{
    return returns.colwise().mean();
}

// run after expected returns
Eigen::RowVectorXd RiskManager::computeVarianceReturns(const Eigen::MatrixXd& returns) const
{
    Eigen::MatrixXd centered = returns.rowwise() - expected_returns;

    return (centered.array().square().colwise().sum()) / (returns.rows()-1);
}

Eigen::MatrixXd RiskManager::computeCovarianceMatrix(const Eigen::MatrixXd& returns) const
{
    Eigen::MatrixXd centered = returns.rowwise() - returns.colwise().mean();
    Eigen::MatrixXd cov = (centered.transpose() * centered) / double(returns.rows() - 1);

    return cov;
}

// the cash value of the equities held in the portfolio (excludes cash)
double RiskManager::getPortfolioEquityValue() const
{
    int num_equities = tickers.size();

    Eigen::VectorXd weights(num_equities);

    for( int i = 0; i < num_equities; i++ )
    {
        weights(i) = p.getNumSharesOf(tickers[i]) * hm.getHistory(tickers[i]).getLastTradePrice();
    }

    return weights.sum();
}

// returns 0 if portfolio is empty
Eigen::VectorXd RiskManager::computePortfolioWeights() const
{
    int num_equities = tickers.size();

    Eigen::VectorXd weights(num_equities);

    for( int i = 0; i < num_equities; i++ )
    {
        weights(i) = p.getNumSharesOf(tickers[i]) * hm.getHistory(tickers[i]).getLastTradePrice();
    }

    if( weights.isZero() )
    {
        return weights;
    }

    weights /= weights.sum();

    return weights;
}

void RiskManager::updateRiskInputs(const int num_snaps)
{
    
    Eigen::MatrixXd returns = computeReturnsMatrix(num_snaps);

    expected_returns = computeExpectedReturns(returns);
    var_returns = computeVarianceReturns(returns).array().sqrt();

    cov_mat = computeCovarianceMatrix(returns);
    w_vec = computePortfolioWeights();
    // portfolio_variance = computePortfolioVariance(cov_mat, w_vec);
}

void RiskManager::updateRisk() 
{
    portfolio_variance = computePortfolioVariance(cov_mat, w_vec);
}

double RiskManager::computePortfolioVariance(const Eigen::MatrixXd& cov_matrix, Eigen::VectorXd& weights) const
{
    return weights.transpose() * cov_matrix * weights;
}

Eigen::VectorXd RiskManager::getEquityPrices() const
{
    int num_equities = getNumEquities();
    Eigen::VectorXd equity_prices(num_equities);

    for( int i = 0; i < num_equities; i++ )
        equity_prices(i) = hm.getHistory(tickers[i]).getLastTradePrice();

    return equity_prices;
}

Eigen::VectorXi RiskManager::computeMaximumAllowableBasketSize(const Eigen::VectorXd& trade_quantities, const int num_snaps) const
{

    const Eigen::VectorXd unit_trade_quantities = trade_quantities/trade_quantities.sum();

    // data to get vector of cash value per equity
    Eigen::VectorXd weights = getPortfolioWeights();
    double value = getPortfolioEquityValue();

    // used for computing portfolio risk of new positions
    Eigen::VectorXd values = weights*value; 

    // used for computing the price of baskets
    Eigen::VectorXd equity_prices = getEquityPrices();

    // compute the maximum number of baskets parchasable by cash (portfolio) and trade value (risk) limitations
    double price_per_basket = unit_trade_quantities.dot(equity_prices); 

    // largest number of shares purchasable with amount of cash available
    double limiting_shares_cash = p.getCash() / price_per_basket;

    // largest cost associated with purchasing basket amounts 
    double largest_cost = (unit_trade_quantities.array() * equity_prices.array()).maxCoeff();

    // largest number of baskets purchasable with maximum trade value (from risk manager)
    double limiting_shares_trade_value = getMaxValuePerTrade() / largest_cost;

    // largest purchasable shares are the minimum of 2 limits
    double maximum_allowable_shares = std::min(limiting_shares_cash, limiting_shares_trade_value);

    double risk = getMaxAllowableRisk() + 1; // +1 to guaruntee loop entrance
    int num_baskets = maximum_allowable_shares + 1; // +1 since loop starts with subtracting 1

    Eigen::VectorXd rounded_basket(getNumEquities());
    Eigen::VectorXd simulated_portfolio_values = values;

    // start with largest number of purchasable baskets
    // subtract until the risk constraints are satisfied or the number of baskets is 0
    for(int i = int(maximum_allowable_shares); risk > getMaxAllowableRisk() && i > 0; i--)
    {
        rounded_basket = (i * unit_trade_quantities).array().floor(); // floor round to ensure price remains below cash

        simulated_portfolio_values = values + rounded_basket; // simulate portfolio weights with basket purchased

        weights = simulated_portfolio_values/simulated_portfolio_values.sum(); // weights is unit portfolio cash values

        risk = std::sqrt( computePortfolioVariance(getCovarianceMatrix(), weights) );
    }


    return rounded_basket.cast<int>();
    
}

std::pair <double, double> RiskManager::computeStopLossTakeProfit(const std::string& ticker, const double price, const double stop_loss_coeff, const double take_profit_coeff) const
{
    int index = getIndexOfTicker(ticker);

    double std_dev = std::sqrt(getVarReturns()[index]) * price;

    double stop_loss = price - stop_loss_coeff * std_dev;

    double take_profit = price + take_profit_coeff * std_dev;

    return {stop_loss, take_profit};
}

} // namespace AlgoTrading
