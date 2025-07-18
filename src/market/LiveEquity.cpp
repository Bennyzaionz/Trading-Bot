#include "market/LiveEquity.h"

namespace AlgoTrading
{

LiveEquity::LiveEquity(const std::string& ticker_, const EquitySnapshot& snap):
ticker(ticker_), currentSnapshot(snap) {}

void LiveEquity::updateEquitySnapshot(const double open_,
                                      const double close_,
                                      const double last_,
                                      const double low_,
                                      const double high_,
                                      const double bid_,
                                      const double ask_,
                                      const int volume_,
                                      const DateTime& dt_)
{
    setOpen(open_),
    setClose(close_),
    setLast(last_);
    setLow(low_);
    setHigh(high_);
    setBid(bid_);
    setAsk(ask_);
    setVolume(volume_);
    setDatetime(dt_);
}

// LiveEquity& LiveEquity::operator=(LiveEquity& other)
// {
//     this.setEquitySnapshot(other.getCurrentSnapshot());
// }

void LiveEquity::print(const PrintType print_type) const
{
    std::cout << "Ticker: " << getTicker() << ", ";

    currentSnapshot.print(print_type);

    // std::cout << std::endl;
}

}