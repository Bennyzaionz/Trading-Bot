#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H

#include "market/HistoricalMarket.h"
#include "trading/Portfolio.h"
#include <utility>
#include <Eigen/Dense>

namespace AlgoTrading {

class RiskManager // base risk manager, can be used out of the box, intended to be inhereted for more complex risk management strategies
{ 
    
    protected:

        // risk parameters
        double max_value_per_trade;
        double max_portfolio_risk;

        // risk inputs
        std::vector <std::string> tickers; // tickers index corresponds to 
        Eigen::MatrixXd cov_mat; // (day, equity)
        Eigen::VectorXd w_vec;
        Eigen::RowVectorXd expected_returns;
        Eigen::RowVectorXd var_returns;

        // risk measurement
        double portfolio_variance;

        // reference data
        const Portfolio& p;
        const HistoricalMarket& hm;

        // ---------- HELPER FUNCTIONS FOR COMPUTING PORTFOLIO VARIANCE ----------
        double getPortfolioEquityValue() const;

        Eigen::VectorXd getDailyLogReturns(const HistoricalEquity& heq, const int num_days) const;

        Eigen::MatrixXd computeReturnsMatrix(const int num_days) const;
        Eigen::RowVectorXd computeExpectedReturns(const Eigen::MatrixXd& returns) const;
        Eigen::RowVectorXd computeVarianceReturns(const Eigen::MatrixXd& returns) const;
        Eigen::MatrixXd computeCovarianceMatrix(const Eigen::MatrixXd& returns) const;

        Eigen::VectorXd computePortfolioWeights() const;
        double computePortfolioVariance(const Eigen::MatrixXd& cov_matrix, Eigen::VectorXd& weights) const;

        // ---------- HELPER FUNCTIONS FOR UPDATING RISK DATA ----------

        int getIndexOfTicker(const std::string& ticker) const;

        Eigen::VectorXd getEquityPrices() const;

    public:

        /*---------- CONSTRUCTOR ----------*/

        RiskManager(const Portfolio& p_, const HistoricalMarket& hm_, double max_value_per_trade_ = 0.05, double max_portfolio_risk_ = 0.02);

        /*---------- STARTUP ----------*/

        /*---------- GETTERS ----------*/

        double getMaxAllowableRisk() const { return max_portfolio_risk; }
        double getMaxValuePerTrade() const { return max_value_per_trade * p.getCashValue(); }

        std::vector <std::string> getTickers() const { return tickers; }
        int getNumEquities() const { return tickers.size(); }

        Eigen::MatrixXd getCovarianceMatrix() const { return cov_mat; }
        Eigen::VectorXd getPortfolioWeights() const { return w_vec; }
        Eigen::VectorXd getExpectedReturns() const{ return expected_returns; }
        Eigen::VectorXd getVarReturns() const { return var_returns; }

        /*---------- SETTERS ----------*/

        void setMaxAllowableRisk(double risk) { max_portfolio_risk = risk; } 
        void setMaxValuePerTrade(double value) { max_value_per_trade = value; }

        /*---------- RISK VALIDATION ----------*/

        virtual double getPortfolioRisk() const { return std::sqrt(portfolio_variance); }
        
        virtual Eigen::VectorXi computeMaximumAllowableBasketSize(const Eigen::VectorXd& trade_quantities, const int num_snaps) const; // based on portfolio standard deviation

        // should be called on new market data before any trades (important to have updated risk data to inform trades)
        virtual void updateRiskInputs(const int num_snaps);

        virtual void updateRisk();

        // computes portfolio risk (may be multiple measures in the future like VaR)
        // should be called before and af
        // virtual void updateRiskMeasures();

        virtual std::pair <double, double> computeStopLossTakeProfit(const std::string& ticker, const double price, const double stop_loss_coeff = 2, const double take_profit_coeff = 2) const;

        // virtual void onMarketUpdate();

        // virtual void onTradeSignal();

};

} // namespace AlgoTrading

#endif // RISK_MANAGER_H
