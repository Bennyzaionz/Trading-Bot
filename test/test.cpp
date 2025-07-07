#include <iostream>
#include <thread>
#include <chrono>

#include "market/HistoricalMarket.h"
#include "market/LiveMarket.h"
#include "trading/Portfolio.h"


int main()
{
    std::cout << "\nStarting test...\n";

    // equity we are interested in
    std::string ticker = std::string("AAPL");
    std::string false_ticker = std::string("FALSE");

    // don't print trades
    const bool verbose = false;

    // Start by creating a Portfolio, Live Market, and Historical Market
    AlgoTrading::Portfolio portfolio(1000000);
    AlgoTrading::LiveMarket live_market;
    AlgoTrading::HistoricalMarket historical_market;

    // Add equity to live market
    live_market.addEquity(ticker);

    // Simulate getting live data from API
    double last = 100;
    double low = 98;
    double high = 104;
    double bid = 99.9;
    double ask = 100.1;
    int volume = 100000;
    AlgoTrading::DateTime dt = AlgoTrading::getCurrentDateTime();

    live_market.updateLiveEquity(ticker, last, low, high, bid, ask, volume, dt);

    historical_market.updateHistoricalMarket(live_market);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // AlgoTrading::TradeStatus status;
    // status = portfolio.marketOrder(AlgoTrading::OrderType::BUY, ticker, live_market, 100, verbose);

    // Simulate getting live data from API
    last = 100;
    low = 98;
    high = 104;
    bid = 100.5;
    ask = 100.7;
    volume = 100000;
    dt = AlgoTrading::getCurrentDateTime();

    live_market.updateLiveEquity(ticker, last, low, high, bid, ask, volume, dt);

    // historical_market.addEquity(ticker);

    historical_market.updateHistoricalMarket(live_market);

    // status = portfolio.marketOrder(AlgoTrading::OrderType::SELL, ticker, live_market, 99, verbose);
    // status = portfolio.marketOrder(AlgoTrading::OrderType::SELL, false_ticker, live_market, 99, verbose);
    // status = portfolio.marketOrder(AlgoTrading::OrderType::SELL, ticker, live_market, 2, verbose);
    // status = portfolio.marketOrder(AlgoTrading::OrderType::SELL, ticker, live_market, 1, verbose);

    portfolio.print();

    portfolio.printLimitOrders();

    historical_market.print();

    AlgoTrading::EquitySnapshot snap;

    /*
    want to be able to call the following:

    portfolio.buyEquity(num_shares, live_equity)

    historical_market.update(live_market)
    
    */

    return 0;
}