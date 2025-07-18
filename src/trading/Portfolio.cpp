#include <algorithm>
#include <iomanip>
#include <unordered_set>

#include "trading/Portfolio.h"

namespace AlgoTrading
{

Portfolio::Portfolio(const double cash_):
    cash(cash_), 
    pos{} {}

double Portfolio::getCommission(int quantity) const
{
    double commission = quantity * COMMISSION_PER_SHARE;

    if( commission <= MIN_COMMISSION )
        return MIN_COMMISSION;
    
    return commission;
}

void Portfolio::addNewPosition(const std::shared_ptr<LiveEquity> leq, 
                               const int num_shares,
                               const double purchase_price,
                               const double stop_loss,
                               const double take_profit,
                               const DateTime& open_dt)
{
    /*
    IMPORTANT: does not check validity of ticker_, only call in TwsApi callback to ensure ticker_ exists
    */

    OpenPosition new_pos(leq, num_shares, purchase_price, stop_loss, take_profit, open_dt);

    pos.push_back(new_pos);
}

void Portfolio::removePosition(const int index)
{
    pos.erase(pos.begin() + index); 
}

void Portfolio::removeShares(const int index, const int num_shares)
{
    pos[index].subtractShares(num_shares);

    if( pos[index].getShares() == 0 )
    {
        removePosition(index);
    }
}

std::vector<std::string> Portfolio::getHoldings() const
{
    std::vector<std::string> holdings = {};

    for( int i = 0; i < getNumPositions(); i++ )
        holdings.push_back(pos[i].getTicker());

    return holdings;
}

std::vector <std::string> Portfolio::getUniqueHoldings() const
{
    std::vector <std::string> holdings = getHoldings();
    std::unordered_set <std::string> unique_set(holdings.begin(), holdings.end());
    return std::vector <std::string> (unique_set.begin(), unique_set.end());
}

std::vector<LiveEquity> Portfolio::getLiveEquities() const
{
    std::vector<LiveEquity> leqs;

    std::vector<OpenPosition> positions = getOpenPositions();

    for( int i = 0; i < getNumPositions(); i++ )
        leqs.push_back(positions[i].getLiveEquity());

    return leqs;
}

std::vector<int> Portfolio::getNumShares() const
{
    std::vector<int> num_shares;

    std::vector<OpenPosition> positions = getOpenPositions();

    for( int i = 0; i < getNumPositions(); i++ )
        num_shares.push_back(positions[i].getShares());

    return num_shares;
}

double Portfolio::getValue() const
{
    double value = getCash();

    for( int i = 0; i < getNumPositions(); i++ )
        value += pos[i].getMarketValue();
    
    return value;
}

std::vector<size_t> Portfolio::getOpenPositionIndeces(const std::string& ticker) const
{
    std::vector<size_t> indeces = {};

    for( int i = 0; i < getNumPositions(); i++)
    {
        if( pos[i].getTicker() == ticker)
            indeces.push_back(i);
    }

    return indeces;
}

void Portfolio::print(const PrintType print_type) const
{
    
    std::cout << std::endl << "---------- Portfolio ----------" << std::endl;
    
    std::cout << std::fixed << std::setprecision(2) << "Cash: $" << cash << std::endl;
    
    std::vector<std::string> tickers = getHoldings();
    
    for( int i = 0; i < getNumPositions(); i++)
    {
        pos[i].getLiveEquity().print(print_type);
        std::cout << ", Shares: " << pos[i].getShares() << std::endl;
    }

    std::cout << "-------------------------------" << std::endl << std::endl;

}

void Portfolio::printPositions() const
{
    std::cout << "---------- Open Positions ----------" << std::endl;
}

int Portfolio::ContainsOpenOrderWithTicker(std::string ticker_) const
{
    std::vector<std::string> holdings = getHoldings();

    auto it = std::find(holdings.begin(), holdings.end(), ticker_);

    if( it != holdings.end() )
        return static_cast<int>(std::distance(holdings.begin(), it));

    return DOES_NOT_CONTAIN;
}

void Portfolio::printLimitOrders() const
{
    std::vector<LimitOrder> open_orders = getLimitOrders();

    std::cout << std::endl << "---------- Open Limit Orders ----------" << std::endl;

    for( int i = 0; i < getNumLimitOrders(); i++)
    {
        open_orders[i].print();
        std::cout << std::endl;
    }

    std::cout << "---------------------------------------" << std::endl << std::endl;
}

int Portfolio::getNumSharesOf(const std::string& ticker) const
{

    std::vector<std::string> tickers = getHoldings();

    std::vector<int> num_shares_vec = getNumShares();

    int num_shares = 0;

    for( int i = 0; i < getNumPositions(); i++ )
    {
        if( tickers[i] == ticker )
            num_shares += num_shares_vec[i];
    }

    return num_shares;
}

TradeStatus Portfolio::buyEquity(const std::shared_ptr<LiveEquity> leq,
                                 const int num_shares_buy, 
                                 const double price, 
                                 const double stop_loss,
                                 const double take_profit,
                                 const DateTime& dt,
                                 const bool verbose)
{
    std::string ticker = leq -> getTicker();
    /*
    Conditions to buy:
    - must have enough money
    - ticker must exist (might check this when I call it in eclient and historical data)
    */

    if( leq == nullptr )
    {
        if ( verbose )
        {
            std::cout << "---------- Order Details ----------" << std::endl;
            std::cout << "Specified equity is not tracked in Live Market" << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }

        return TradeStatus::TICKER_NOT_TRACKED;
    }

    double commission = getCommission(num_shares_buy);

    double cost = num_shares_buy * price + commission;

    if ( cash <= cost )
    {
        if ( verbose )
        {
            std::cout << "---------- Purchase Details ----------" << std::endl;
            std::cout << "Insufficient Funds" << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }

        return TradeStatus::INSUFFICIENT_FUNDS;
    }

    cash -= cost; // pay for stock + commission

    addNewPosition(leq, num_shares_buy, price, stop_loss, take_profit, dt);

    if( verbose )
    {
        std::cout << "---------- Purchase Details ----------" << std::endl;
        std::cout << "Ticker: " << ticker 
                  << ", Number of Shares: " << num_shares_buy
                  << ", Price Per Share: " << price
                  << ", Gross Cost: " << (num_shares_buy * price) 
                  << ", Commission: " << commission 
                  << ", Total Cost: " << cost << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }

    return TradeStatus::SUCCESSFUL_TRADE;
}

TradeStatus Portfolio::sellEquity(const int index, const int num_shares_sell, const double price, const bool verbose)
{
    /*
    Conditions to sell:
    - must have enough shares
    */

    std::string ticker = pos[index].getTicker();

    // handle index out of bounds
    if( index >= getNumPositions() || index == DOES_NOT_CONTAIN)
    {
        if ( verbose )
        {
            std::cout << "---------- Sale Details ----------" << std::endl;
            std::cout << "Index is out of bounds, tried to sell index:: " << index 
                      << ", Number of positions: " << getNumPositions() << std::endl; 
            std::cout << "--------------------------------------" << std::endl;
        }

        return TradeStatus::SELL_INDEX_OUT_OF_BOUNDS; 
    }

    // handle trying to sell more shres than held (can handle shorting separately so it is not done accidently)
    else if( pos[index].getShares() < num_shares_sell )
    {
        if ( verbose )
        {
            std::cout << "---------- Sale Details ----------" << std::endl;
            std::cout << "Insufficient Shares, Number of Shares Held: " << pos[index].getShares() 
                      << ", Number of Shares Attempted to Sell: " << num_shares_sell << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }
        
        return TradeStatus::INSUFFICIENT_FUNDS;
    }

    // enough shares of position at index to sell

    double commission = getCommission(num_shares_sell);

    double proceeds = num_shares_sell*price - commission;

    cash += proceeds; // add cash from sale

    removeShares(index, num_shares_sell);

    // removeEquity(ticker_, num_shares_sell); // remove shares

    if( verbose )
    {
        std::cout << "---------- Sale Details ----------" << std::endl;
        std::cout << "Ticker: " << ticker
                  << ", Number of Shares: " << num_shares_sell
                  << ", Gross Proceeds: " << (num_shares_sell * price) 
                  << ", Commission: " << commission 
                  << " Net Proceeds: " << proceeds << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }
    return TradeStatus::SUCCESSFUL_TRADE;
}

TradeStatus Portfolio::attemptSellNumShares(const std::string& ticker, 
                                            const int num_shares_attempt_sell, 
                                            const double price, 
                                            const bool verbose)
{

    std::vector<size_t> indeces = getOpenPositionIndeces(ticker);

    int shares_sold = 0;

    int remaining = num_shares_attempt_sell; // remaining shares to sell in order

    int position_size;

    for( int j = 0; j < indeces.size() && shares_sold < num_shares_attempt_sell; j++ )
    {
        position_size = pos[j].getShares();
        if( remaining > position_size )
        {
            // sell position size
            sellEquity(indeces[j], position_size, price, verbose);
            shares_sold += position_size;
        }
        else
        {
            // sell remaining 
            sellEquity(indeces[j], remaining, price, verbose);
            shares_sold += remaining;
        }
    }  
    
    if( shares_sold == num_shares_attempt_sell)
    {
        return TradeStatus::SUCCESSFUL_TRADE;
    }
    else if( shares_sold <= num_shares_attempt_sell )
    {
        return TradeStatus::PARTIAL_FILL;
    }
    return TradeStatus::ERROR;
}

TradeStatus Portfolio::marketOrder(const OrderType order_type,
                                   const std::shared_ptr<LiveEquity> leq, 
                                   const int num_shares,
                                   const bool verbose)
{
    std::string ticker = leq -> getTicker();

    if( verbose )
        std::cout << std::endl << "Market Order: " << std::endl;

    if( order_type == OrderType::BUY )
    {
        double buy_price = leq -> getAsk();
        double stop_loss = buy_price - 10;
        double take_profit = buy_price + 10;
        DateTime& dt = leq -> getDatetime();
        return buyEquity(leq, num_shares, buy_price, stop_loss, take_profit, dt, verbose);
    }

    else if( order_type == OrderType::SELL)
    {
        int index = ContainsOpenOrderWithTicker(ticker);
        double sell_price = leq -> getBid();
        return sellEquity(index, num_shares, sell_price, verbose);
    }

    return TradeStatus::ERROR;
}

TradeStatus Portfolio::marketOrder(const OrderType order_type,
                                   const std::string ticker,
                                   const LiveMarket& lm, 
                                   const int num_shares,
                                   const bool verbose)
{

    std::shared_ptr<LiveEquity> leq = lm.getEquity(ticker);

    if( leq == nullptr )
    {
        if ( verbose )
        {
            std::cout << std::endl << "Market Order: " << std::endl;
            std::cout << "---------- Purchase Details ----------" << std::endl;
            std::cout << "Specified ticker " << ticker << " is not tracked in Live Market" << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }

        return TradeStatus::TICKER_NOT_TRACKED;
    }
        
    return marketOrder(order_type, leq, num_shares, verbose);

}
TradeStatus Portfolio::limitOrder(const OrderType order_type,
                                  const std::string&ticker,
                                  const int num_shares,
                                  const int desired_price,
                                  const DateTime& dt,
                                  const DateTime& exp,
                                  const bool verbose)
{
    // check buy conditions:
    // - enough money
    if( order_type == OrderType::BUY && cash < num_shares * desired_price + getCommission(num_shares))
    {  
        if( verbose )
        {
            std::cout << "\n---------- LIMIT ORDER ----------\n" 
                      << "Ticker: " << ticker
                      << ", INSUFFICIENT FUNDS\n"
                      << "---------------------------------\n";
        } 
        return TradeStatus::INSUFFICIENT_FUNDS;
    }

    // check sell conditions:
    // - enough shares
    else if( order_type == OrderType::SELL && getNumSharesOf(ticker) < num_shares )
    {
        if( verbose )
        {
            std::cout << "\n---------- LIMIT ORDER ----------\n" 
                      << "Ticker: " << ticker
                      << ", INSUFFICIENT SHARES\n"
                      << "---------------------------------\n";
        } 
        return TradeStatus::INSUFFICIENT_SHARES;
    }


    LimitOrder order(ticker, order_type, desired_price, num_shares, dt, exp);
    
    orders.push_back(order);
    if( verbose )
    {    
        std::string ot = (order_type == OrderType::BUY) ? "BUY" : "SELL";

        std::cout << "\n---------- LIMIT ORDER ----------\n" 
                  << "Order Type: " << ot
                  << ", Ticker: " << ticker
                  << ", Number of Shares: " << num_shares
                  << ", Limit Price: " << desired_price
                  << ", Placed at: " << dt.toString()
                  << ", Expires at: " << exp.toString()
                  << "\n---------------------------------\n";
    }

    return TradeStatus::SUCCESSFUL_TRADE;
}

TradeStatus Portfolio::limitOrder(const OrderType order_type,
                                  const LiveEquity& leq,
                                  const int num_shares,
                                  const int desired_price,
                                  const DateTime& dt_placed,
                                  const DateTime& exp,
                                  const bool verbose)
{
    const std::string ticker = leq.getTicker();

    // check buy conditions:
    // - enough money
    if( order_type == OrderType::BUY && cash < num_shares * desired_price)
        return TradeStatus::INSUFFICIENT_FUNDS;

    // check sell conditions:
    // - enough shares
    else if( order_type == OrderType::SELL && getNumSharesOf(ticker) < num_shares )
        return TradeStatus::INSUFFICIENT_SHARES;

    LimitOrder order(ticker, order_type, desired_price, num_shares, dt_placed, exp);
    
    orders.push_back(order);

    return TradeStatus::SUCCESSFUL_TRADE;
}

UpdateType Portfolio::updateLiveEquity(const std::string& ticker,
                                       const double open_,
                                       const double close_,
                                       const double last_,
                                       const double low_,
                                       const double high_,
                                       const double bid_,
                                       const double ask_,
                                       const int volume_,
                                       const DateTime& dt_)
{
    int index = ContainsOpenOrderWithTicker(ticker);

    if( index == DOES_NOT_CONTAIN )
        return UpdateType::TICKER_NOT_IN_PORTFOLIO;

    pos[index].updateLiveEquity(open_, close_, last_, low_, high_, bid_, ask_, volume_, dt_);

    return UpdateType::SUCCESSFUL_UPDATE;
}

void Portfolio::removeLimitOrder(const int index)
{
    orders.erase(orders.begin() + index);
}

void Portfolio::deleteExpiredLimitOrders(const DateTime& dt_curr, const bool verbose)
{
    if( verbose )
    {
        std::cout << "\n---------- EXPIRED LIMIT ORDERS ----------\n";
    }

    for( int i = getNumLimitOrders() - 1; i >= 0 ; i-- ) // iterate backwards through list
    {
        if( orders[i].getExpiryTime() >= dt_curr )
        {
            if( verbose )
            {
                orders[i].print();
                std::cout << std::endl;
            }            
            removeLimitOrder(i);
        }
    }
    if( verbose )
    {
        std::cout << "------------------------------------------\n";
    }
}

int Portfolio::getFillVolume(const OrderType order_type,
                                const double order_price,
                                const double live_low,
                                const double live_high,  
                                const int live_volume     
                               )
{
    double fill_range;
    double fill_ratio;
    double fill_volume;
    double live_price_range = live_high - live_low;

    if( order_type == OrderType::BUY)
    {
        fill_range = order_price - live_low;
        fill_ratio = fill_range/live_price_range; // ratio of trade volume available to fill
        fill_volume = fill_ratio * live_volume;
    }
    else
    {
        fill_range = live_high - order_price;
        fill_ratio = fill_range/live_price_range; // ratio of trade volume available to fill
        fill_volume = fill_ratio * live_volume;
    }

    return int(fill_volume);
}

double Portfolio::getFillPrice(const OrderType order_type, 
                               const double live_low, 
                               const double live_high, 
                               const double order_price)
{
    double fill_price = (order_type == OrderType::BUY) ? (live_low + order_price)/2 : (live_high + order_price)/2;
    return fill_price;
}

void Portfolio::executeLimitOrders(const LiveMarket& lm, const bool verbose)
{
    /*
    execute buy orders if:
    buy: limit price >= low
    sell: limit price <= high
    */

    if( verbose )
    {
        std::cout << "\nEXECUTING LIMIT ORDERS...\n\n";
    }

    // live data
    int live_volume;
    double live_low;
    double live_high;
    EquitySnapshot live_snap;

    // order data
    std::string ticker;
    int order_volume;
    OrderType order_type;
    double order_price; // price listed in order

    // fill data
    double fill_price; // average price of filled share
    int fill_volume; // volume available to fill
    int volume; // final volume filled (min of fill_volume and order_volume)

    // std::cout << "Num Limit Orders: " << getNumLimitOrders() << std::endl;

    for( int i = getNumLimitOrders() - 1; i >= 0; i-- )
    {
        // get live snapshot for current order
        ticker = orders[i].getTicker();
        live_snap = lm.getEquity(ticker) -> getCurrentSnapshot();

        // keep high and low prices
        live_low = live_snap.getLow();
        live_high = live_snap.getHigh();

        // data to simulate available, fillable volume assuming uniform distribution of prices
        order_price = orders[i].getPrice();
        order_volume = orders[i].getQuantity();
        order_type = orders[i].getType();
        live_volume = live_snap.getVolume();

        // fill info
        fill_volume = getFillVolume(order_type, order_price, live_low, live_high, live_volume); // fillable volume
        fill_price = getFillPrice(order_type, live_low, live_high, order_price); // average fill price, need to fix for sells still
        volume = (order_volume <= fill_volume) ? order_volume : fill_volume; // actual volume filled is minimum

        if( orders[i].getType() == OrderType::BUY && order_price >= live_low ) // buy && price below order price
        {

            buyEquity(lm.getEquity(ticker), 
                    volume,
                    fill_price,
                    100, // stop loss
                    100, // take profit
                    live_snap.getDateTime(), 
                    verbose);

            removeLimitOrder(i);
        }

        else if( orders[i].getType() == OrderType::SELL && order_price <=  live_high ) // sell && price is above order price
        {

            attemptSellNumShares(ticker, volume, fill_price, verbose);

            removeLimitOrder(i);
        }
    }
    if( verbose )
    {
        std::cout << "\nDONE EXECUTING LIMIT ORDERS...\n";
    }
}

void Portfolio::executeStopLossTakeProfit(const bool is_live, const LiveMarket&lm, const bool verbose)
{
    std::vector<OpenPosition> positions = getOpenPositions();

    std::string ticker;

    double stop_loss, take_profit;

    int num_shares;

    if( is_live )
    {
        double live_ask, live_bid;
        for( int i = getNumPositions() - 1; i >= 0; i--)
        {
            ticker = positions[i].getTicker();
            live_ask = lm.getEquity(ticker) -> getAsk();
            live_bid = lm.getEquity(ticker) -> getBid();
            stop_loss = positions[i].getStopLoss();
            take_profit = positions[i].getTakeProfit();
            num_shares = positions[i].getShares();
            // stop loss

            if( live_bid < stop_loss || live_ask > take_profit)
            {
                marketOrder(OrderType::SELL, lm.getEquity(ticker), num_shares, verbose);
            }
        }
    }
    // historical
    else
    {
        double live_low, live_high;
        for( int i = getNumPositions() - 1; i >= 0; i--)
        {
            ticker = positions[i].getTicker();
            live_low = lm.getEquity(ticker) -> getLow();
            live_high = lm.getEquity(ticker) -> getHigh();
            stop_loss = positions[i].getStopLoss();
            take_profit = positions[i].getTakeProfit();
            num_shares = positions[i].getShares();

            // assumes that we can sell at the stop loss/take profit
            // not always possible, might change later
            // since we only have low, high, last, bid, ask for time step
            // we say "we would have sold if we had the tick price when it crossed"
            if( live_low < stop_loss )
            {
                if( verbose )
                {
                    std::cout << "Stop Loss Hit..." << std::endl;
                }
                sellEquity(i, num_shares, stop_loss, verbose);
            }
            else if( live_high > take_profit )
            {
                if( verbose )
                {
                    std::cout << "Take Profit Hit..." << std::endl;
                } 
                sellEquity(i, num_shares, take_profit, verbose);
            }
        }
    }
}

} // end namespace