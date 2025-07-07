#include "actor/Actor.h"

namespace AlgoTrading {

Actor::Actor(double cash,
             std::shared_ptr<Strategy> strategy,
             std::shared_ptr<RiskManager> riskManager,
             std::shared_ptr<HistoricalMarket> marketData)
    : portfolio(Portfolio(cash)),
      strategy(std::move(strategy)),
      riskManager(std::move(riskManager)),
      marketData(std::move(marketData))
{}

} // namespace AlgoTrading