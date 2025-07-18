#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "market/HistoricalMarket.h"
#include "market/LiveMarket.h"
#include "trading/Portfolio.h"
#include "risk/RiskManager.h"

int main()
{
    std::cout << "\nStarting test...\n";

    // equity we are interested in
    std::string ticker = std::string("AAPL");
    std::string false_ticker = std::string("FALSE");
    AlgoTrading::TradeStatus status;

    // // don't print trades
    // const bool verbose = false;

    // // Start by creating a Portfolio, Live Market, and Historical Market
    // AlgoTrading::Portfolio portfolio(1000000);
    // AlgoTrading::LiveMarket live_market;
    // AlgoTrading::HistoricalMarket historical_market;

    // // Add equity to live market
    // live_market.addEquity(ticker);

    // // Simulate getting live data from API
    // double open = 100;
    // double close = 100;
    // double last = 100;
    // double low = 98;
    // double high = 104;
    // double bid = 99.9;
    // double ask = 100.1;
    // int volume = 100000;
    // AlgoTrading::DateTime dt = AlgoTrading::getCurrentDateTime();

    // live_market.updateLiveEquity(ticker, open, close, last, low, high, bid, ask, volume, dt);

    // live_market.print();

    // historical_market.updateHistoricalMarket(live_market);

    // status = portfolio.marketOrder(AlgoTrading::OrderType::BUY,
    //                                live_market.getEquity(ticker),
    //                                10,
    //                                true);

    // // status = portfolio.limitOrder(AlgoTrading::OrderType::BUY,
    // //                               ticker, 
    // //                               5, 
    // //                               105, 
    // //                               AlgoTrading::getCurrentDateTime(),
    // //                               AlgoTrading::getCurrentDateTime(),
    // //                               true); 
                                  
    // // portfolio.printLimitOrders();

    // // portfolio.executeLimitOrders(live_market, true);
                                  
    // portfolio.print();

    // // std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // // status = portfolio.marketOrder(AlgoTrading::OrderType::BUY, ticker, live_market, 100, verbose);

    // // Simulate getting live data from API
    // open = 100;
    // close = 100;
    // last = 100;
    // low = 120;
    // high = 150;
    // bid = 85;
    // ask = 84;
    // volume = 100000;
    // dt = AlgoTrading::getCurrentDateTime();

    // live_market.updateLiveEquity(ticker, open, close, last, low, high, bid, ask, volume, dt);

    // live_market.print();

    // historical_market.updateHistoricalMarket(live_market);

    // status = portfolio.marketOrder(AlgoTrading::OrderType::SELL,
    //                                live_market.getEquity(ticker),
    //                                10,
    //                                true);

    // create instances of classes 
    AlgoTrading::HistoricalMarket hm;
    AlgoTrading::LiveMarket lm;
    AlgoTrading::Portfolio p(1000000);

    // simulate historical market data
    std::vector <std::string> tickers = {"AAPL", "GOOG", "NVDA", "AMZN", "META"};
    hm.simulateData(tickers, 25);
    // hm.print(AlgoTrading::PrintType::OHLC);

    // simulate live market data
    AlgoTrading::DateTime now = AlgoTrading::getCurrentDateTime();
    for( int i = 0; i < tickers.size(); i++ )
    {
        lm.addEquity(tickers[i]);
        lm.updateLiveEquity(tickers[i], 100, 100, 100, 100, 100, 100, 100, 100, now);
    }

    lm.print();

    // p.marketOrder(AlgoTrading::OrderType::BUY, lm.getEquity(tickers[0]), 100, true);

    for( int i = 0; i < tickers.size(); i++ )
    {
        p.marketOrder(AlgoTrading::OrderType::BUY, lm.getEquity(tickers[i]), (i+1)*(i+1), true);
        p.marketOrder(AlgoTrading::OrderType::BUY, lm.getEquity(tickers[i]), i+1, true);
    }

    AlgoTrading::RiskManager rm;

    double portfolio_risk = rm.compute_portfolio_risk(p, hm);

    std::cout << std::endl << "Portfolio Risk: " << portfolio_risk << std::endl;

    return 0;
}