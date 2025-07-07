#include <algorithm>
#include <iomanip>

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

void Portfolio::removePosition(int index)
{
    pos.erase(pos.begin() + index); 
    // since this is being accessed by a loop, 
    // might have to add indeces that we wish 
    // to delete to a queue to delete after loop
}

void Portfolio::removeShares(const int index, const int num_shares)
{
    pos[index].subtractShares(num_shares);

    // std::cout << pos.empty() << std::endl;

    // need to handle logic of erasing position once it is at 0 shares
    // been having an error with vector indeces when I try deleting
}

std::vector<std::string> Portfolio::getHoldings() const
{
    std::vector<std::string> holdings = {};

    for( int i = 0; i < getNumPositions(); i++)
        holdings.push_back(pos[i].getTicker());

    return holdings;
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

    std::cout << "---------- Open Limit Orders ----------" << std::endl;

    for( int i = 0; i < getNumLimitOrders(); i++)
    {
        open_orders[i].print();
    }

    std::cout << "---------------------------------------" << std::endl << std::endl;
}

int Portfolio::getNumSharesOf(const std::string& ticker)
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
        std::cout << "Ticker: " << pos[index].getTicker()
                  << ", Number of Shares: " << num_shares_sell
                  << ", Gross Proceeds: " << (num_shares_sell * price) 
                  << ", Commission: " << commission 
                  << " Net Proceeds: " << proceeds << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }
    return TradeStatus::SUCCESSFUL_TRADE;
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
        return TradeStatus::INSUFFICIENT_FUNDS;

    // check sell conditions:
    // - enough shares
    else if( order_type == OrderType::SELL && getNumSharesOf(ticker) < num_shares )
        return TradeStatus::INSUFFICIENT_SHARES;


    LimitOrder order(ticker, order_type, desired_price, num_shares, dt, exp);
    
    orders.push_back(order);

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

    pos[index].updateLiveEquity(last_, low_, high_, bid_, ask_, volume_, dt_);

    return UpdateType::SUCCESSFUL_UPDATE;
}

} // end namespace