#ifndef LIMIT_ORDER_H
#define LIMIT_ORDER_H

#include <string>
#include "core/DateTime.h"

namespace AlgoTrading {

enum class OrderType { BUY, SELL };

class LimitOrder {
private:
    std::string ticker;
    OrderType type;
    double price;
    int quantity;
    DateTime creation_time;
    DateTime expiry_time;

public:
    // Constructor
    LimitOrder(const std::string& ticker_,
               OrderType type_,
               double price_,
               int quantity_,
               const DateTime& creation_time_,
               const DateTime& expiry_time_);

    // Getters
    std::string getTicker() const { return ticker; }
    OrderType getType() const { return type; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    DateTime getCreationTime() const { return creation_time; }
    DateTime getExpiryTime() const { return expiry_time; }

    // Logic
    bool isExpired(const DateTime& current_time) const;
    bool shouldFill(double last) const;

    // Utility
    void print() const;
};

} // namespace AlgoTrading

#endif