#include "trading/OpenPosition.h"

namespace AlgoTrading {

OpenPosition::OpenPosition(const std::shared_ptr<LiveEquity>& live_equity_,
                           int shares_,
                           double entry_price_,
                           double stop_loss_,
                           double take_profit_,
                           const DateTime& open_dt_)
    : live_equity(live_equity_),
      shares(shares_),
      entry_price(entry_price_),
      stop_loss(stop_loss_),
      take_profit(take_profit_),
      open_dt(open_dt_) {}

OpenPosition::OpenPosition(const std::string& ticker_,
                           int shares_,
                           double entry_price_,
                           double stop_loss_,
                           double take_profit_,
                           const DateTime& open_dt_)
    : live_equity(std::make_shared<LiveEquity>(ticker_)),
      shares(shares_),
      entry_price(entry_price_),
      stop_loss(stop_loss_),
      take_profit(take_profit_),
      open_dt(open_dt_) {}

void OpenPosition::updateLiveEquity(const double last_,
                                    const double low_,
                                    const double high_,
                                    const double bid_,
                                    const double ask_,
                                    const int volume_,
                                    const DateTime& dt_)
{
    live_equity->updateEquitySnapshot(last_, low_, high_, bid_, ask_, volume_, dt_);
}                                    

bool OpenPosition::hitStopLoss() const {
    double last_price = getLiveEquity().getLast();
    return (shares > 0) ? (last_price <= stop_loss) : (last_price >= stop_loss);
}

bool OpenPosition::hitTakeProfit() const {
    double last_price = getLiveEquity().getLast();
    return (shares > 0) ? (last_price >= take_profit) : (last_price <= take_profit);
}

void OpenPosition::print() const
{
    // getLiveEquity().print(print_type);

    std::cout << "Ticker: " << getTicker()
              << ", Position Opened at: " << getOpenDT().toString()
              << ", Number of Shares: " << getShares()
              << ", Entry Price: $" << getEntryPrice()
              << ", Stop Loss: $" << getStopLoss()
              << ", Take Profit: $" << getTakeProfit()
              << std::endl;
}

}