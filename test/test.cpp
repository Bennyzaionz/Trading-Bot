#include <iostream>
#include <string>

#include "Portfolio.h"


int main()
{

    // // std::cout << "Hello" << std::endl;

    // AlgoTrading::EquitySnapshot h("20201201", 1, 2, 3, 4, 5, 6, 7, 8, 9);

    // AlgoTrading::HistoricalEquityData hd("AAPL", "M", 1);

    // for ( int i = 0; i < 5; i++)
    // {
    //     hd.append_data(h);
    // }

    // // std::cout << h.get_datetime() << std::endl;

    // // std::cout << h.get_volume() << std::endl;

    // // std::cout << hd.get_size() << std::endl;

    // hd.print("BID-ASK");

    // Equity aapl = Equity()

    AlgoTrading::Equity aapl("AAPL", 1, 2, 3, 4, 5, 6, 7);

    AlgoTrading::Equity goog("GOOG", 1, 2, 3, 4, 5, 6, 7);

    AlgoTrading::Portfolio p(10000);

    p.print();

    // std::cout << "Buying Apple..." << std::endl;

    int trade_status = p.buyEquity(aapl, 50, 100, true);

    p.print();

    trade_status = p.buyEquity(goog, 10, 200, true);

    // std::cout << "Successful Trade: " << (trade_status == AlgoTrading::SUCCESSFUL_TRADE) << std::endl; 

    p.print();

    trade_status = p.sellEquity("GOOG", 5, 205, true);

    p.print();

    return 0;
}