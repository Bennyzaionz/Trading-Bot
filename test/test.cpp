#include <iostream>
#include <string>

#include "Portfolio.h"
#include "HistoricalEquityData.h"
#include "DateTime.h"

int main()
{

    // std::string d = "20250619 01:00:00";

    // // AlgoTrading::DateTime dt = (d);

    // AlgoTrading::DateTime dt = (std::string("20250619"));

    // dt.print();

    // dt = dt + 600;

    // dt.print();

    AlgoTrading::EquitySnapshot snap("20250618", 100, 99.5, 101.25, 99.8, 99.9, 120);

    // snap.print();

    AlgoTrading::LiveEquity aapl("AAPL", snap);

    // aapl.print();

    // AlgoTrading::HistoricalEquityData hist("AAPL", AlgoTrading::HOURS, 1);

    // hist.append_data(aapl);

    // hist.append_data(aapl);

    // hist.append_data(aapl);

    // hist.print();

    AlgoTrading::Portfolio p(100000);

    p.print();

    p.buyEquity(aapl, 100, 205.54, true);

    p.print();

    p.buyEquity(aapl, 50, 200, true);

    p.print();

    p.sellEquity("AAPL", 150, 210, true);

    p.print();

    return 0;
}