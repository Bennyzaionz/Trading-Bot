#ifndef ACTOR_H
#define ACTOR_H

#include "trading/Portfolio.h"
#include "strategy/Strategy.h"
#include "risk/RiskManager.h"
#include "market/HistoricalMarket.h"

namespace AlgoTrading
{

class Actor
{

    protected:

        std::shared_ptr<Strategy> strategy;
        std::shared_ptr<RiskManager> riskManager;
        std::shared_ptr<HistoricalMarket> marketData; 

        Portfolio portfolio;

    public:

        Actor(double cash,
              std::shared_ptr<Strategy> strategy,
              std::shared_ptr<RiskManager> riskManager,
              std::shared_ptr<HistoricalMarket> marketData);

        virtual ~Actor() = default;

        
    // Pure virtual: derived classes must implement this main logic per tick
    virtual void onTick() = 0;

    // Optional: lifecycle hooks
    virtual void onStart() {}
    virtual void onStop() {}

    // Accessors
    Portfolio& getPortfolio() { return portfolio; }
    const Portfolio& getPortfolio() const { return portfolio; }

    std::shared_ptr<Strategy> getStrategy() const { return strategy; }
    void setStrategy(std::shared_ptr<Strategy> strat) { strategy = strat; }

    std::shared_ptr<RiskManager> getRiskManager() const { return riskManager; }
    void setRiskManager(std::shared_ptr<RiskManager> rm) { riskManager = rm; }

};

}

#endif