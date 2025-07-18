#include "actor/Actor.h"

#ifndef HISTORICAL_ACTOR_H
#define HISTORICAL_ACTOR_H

namespace AlgoTrading {

class HistoricalActor : public Actor 
{

private: 

public:
    // Constructor passes pointers to base class constructor
    HistoricalActor(std::shared_ptr<Strategy> strategy,
                    std::shared_ptr<RiskManager> riskManager,
                    std::shared_ptr<HistoricalMarket> historicalData);

    void onTick(const DateTime& dt, const LiveMarket& lm) override;

    void onStart() override;
    void onStop() override;
};

} // namespace AlgoTrading

#endif // HISTORICAL_ACTOR_H
