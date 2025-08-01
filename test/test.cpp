#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "market/HistoricalMarket.h"
#include "market/LiveMarket.h"
#include "trading/Portfolio.h"
#include "risk/RiskManager.h"
// #include "Eigen/Dense"

int main()
{
    std::cout << "\nStarting test...\n";

    // equity we are interested in
    std::string ticker = std::string("AAPL");
    std::string false_ticker = std::string("FALSE");
    AlgoTrading::TradeStatus status;

    // // don't print trades
    // const bool verbose = false;

    // create instances of classes
    // order is important since classes reference other classes in their construction
    AlgoTrading::HistoricalMarket hm;
    AlgoTrading::LiveMarket lm;
    AlgoTrading::Portfolio p(hm, 1000000);

    // simulate historical market data
    std::vector <std::string> tickers = {"AAPL", "GOOG", "NVDA", "AMZN", "META"};
    hm.simulateData(tickers, 25);

    AlgoTrading::RiskManager rm(p, hm, 0.05, 0.012);

    // simulate live market data
    AlgoTrading::DateTime now = AlgoTrading::getCurrentDateTime();
    for( int i = 0; i < tickers.size(); i++ )
    {
        lm.addEquity(tickers[i]);
        lm.updateLiveEquity(tickers[i], 100, 100, 100, 100, 100, 100, 100, 100, now);
    }

    lm.print(AlgoTrading::PrintType::TRADE);

    rm.updateRiskInputs(20);

    // p.marketOrder(AlgoTrading::OrderType::BUY, lm.getEquity(tickers[0]), 100, true);

    for( int i = 0; i < tickers.size(); i++ )
    {
        // p.marketOrder(AlgoTrading::OrderType::BUY, lm.getEquity(tickers[i]), (i+1)*(i+1), true);
        // p.marketOrder(AlgoTrading::OrderType::BUY, lm.getEquity(tickers[i]), i+1, true);

        p.marketOrder(AlgoTrading::OrderType::BUY, tickers[i], (i+1)*(i+1), rm, true);
        p.marketOrder(AlgoTrading::OrderType::BUY, tickers[i], (i+1)*(i+1), rm, true);
    }

    rm.updateRiskInputs(20);

    rm.updateRisk();

    // std::cout << "weights: \n" << rm.getPortfolioWeights() << std::endl;

    // std::cout << "cov: \n" << rm.getCovarianceMatrix() << std::endl;

    // std::cout << std::endl << "Cash Available: " << p.getCash() << std::endl;

    Eigen::VectorXd basket(tickers.size());

    for( int i = 0; i < tickers.size(); i++ )
    {
        basket(i) = double(i)/(1 + i*i*i);
    }

    Eigen::VectorXi max_basket_size = rm.computeMaximumAllowableBasketSize(basket, 20);

    std::cout << "max pos size: \n" << max_basket_size << std::endl;

    // std::pair <double, double> stops = rm.computeStopLossTakeProfit(tickers[0], 100);

    // std::cout << "stops: " << stops.first << ", " << stops.second << std::endl;

    std::cout << "Portfolio Risk: " << rm.getPortfolioRisk() << std::endl << std::endl;

    p.print();

    std::cout << "Portfolio Risk: " << rm.getPortfolioRisk() << std::endl << std::endl;

    // hm.print();

    std::cout << "Num Shares of GOOG: " << p.getNumSharesOf(tickers[1]) << std::endl;

    return 0;
}