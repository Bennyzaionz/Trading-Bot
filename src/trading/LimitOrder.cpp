#include "trading/LimitOrder.h"
#include <iostream>

namespace AlgoTrading {

LimitOrder::LimitOrder(const std::string& ticker_,
                       OrderType type_,
                       double price_,
                       int quantity_,
                       const DateTime& creation_time_,
                       const DateTime& expiry_time_)
    : ticker(ticker_), type(type_), price(price_),
      quantity(quantity_), creation_time(creation_time_), expiry_time(expiry_time_) {}

bool LimitOrder::isExpired(const DateTime& current_time) const {
    return current_time > expiry_time;
}

bool LimitOrder::shouldFill(double last) const {
    if (type == OrderType::BUY)
        return last <= price;
    else
        return last >= price;
}

void LimitOrder::print() const {
    std::cout << "["
              << (type == OrderType::BUY ? "BUY" : "SELL")
              << " " << quantity << " " << ticker
              << " @ " << price << "] "
              << "Created: " << creation_time.toString()
              << " | Expires: " << expiry_time.toString()
              << std::endl;
}

} // namespace AlgoTrading
