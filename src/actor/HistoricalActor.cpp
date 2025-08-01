#include "actor/HistoricalActor.h"

namespace AlgoTrading
{

void HistoricalActor::onTick(const DateTime& dt, const LiveMarket& lm)
{
    /*
    - execute stop loss and take profit
    - delete expired limit orders
    - execute limit orders
    - execute strategy based on updated data
    - reassess risk
    */

    bool verbose = true;

    portfolio.executeStopLossTakeProfit(false, lm, *riskManager, verbose);

    portfolio.deleteExpiredLimitOrders(dt, verbose);

    portfolio.executeLimitOrders(lm, *riskManager, verbose);

    
}

void HistoricalActor::onStart()
{
    std::cout << "Starting Historical Actor..." << std::endl;
}

void HistoricalActor::onStop()
{
    std::cout << "Stopping Historical Actor..." << std::endl;
}


}