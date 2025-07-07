#include "actor/Actor.h"

#ifndef HISTORICAL_ACTOR_H
#define HISTORICAL_ACTOR_H

#include "Actor.h"

namespace AlgoTrading {

class HistoricalActor : public Actor {
public:
    // Constructor passes pointers to base class constructor
    HistoricalActor(std::shared_ptr<Strategy> strategy,
                    std::shared_ptr<RiskManager> riskManager,
                    std::shared_ptr<HistoricalMarket> historicalData);

    void onTick(const std::string& ticker) override;
};

} // namespace AlgoTrading

#endif // HISTORICAL_ACTOR_H
