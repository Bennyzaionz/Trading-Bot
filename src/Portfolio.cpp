#include <algorithm>

#include "Portfolio.h"

namespace AlgoTrading
{

Portfolio::Portfolio(const double cash_):
    cash(cash_), 
    equities{}, 
    num_shares{} {}


std::vector<std::string> Portfolio::getHoldings() const
{
    std::vector<std::string> holdings = {};

    for( int i = 0; i < equities.size(); i++)
        holdings.push_back(equities[i].getTicker());

    return holdings;
}

double Portfolio::getValue() const
{
    double value = cash;

    for( int i = 0; i < equities.size(); i++)
        value += equities[i].getLast() * num_shares[i];
    
    return value;
}

double Portfolio::getCommission(int quantity) const
{
    double commission = quantity * COMMISSION_PER_SHARE;

    if( commission <= MIN_COMMISSION )
        return MIN_COMMISSION;
    
    return commission;
}

void Portfolio::print(int print_type) const
{
    
    std::cout << std::endl << "---------- Portfolio ----------" << std::endl;
    
    std::cout << "Cash: $" << cash << std::endl;
    std::vector<std::string> tickers = getHoldings();
    
    for( int i = 0; i < getSize(); i++)
    {
        equities[i].print(print_type);
        std::cout << ", Shares: " << num_shares[i] << std::endl;
    }

    std::cout << "-------------------------------" << std::endl;

}

int Portfolio::containsTicker(std::string ticker_) const
{
    std::vector<std::string> holdings = getHoldings();

    auto it = std::find(holdings.begin(), holdings.end(), ticker_);

    if( it != holdings.end() )
        return static_cast<int>(std::distance(holdings.begin(), it));

    return DOES_NOT_CONTAIN;
}

void Portfolio::addEquity(std::string ticker_, int quantity)
{
    /*
    IMPORTANT: does not check validity of ticker_, only call in TwsApi callback to ensure ticker_ exists
    */
    int index = containsTicker(ticker_);
    
    if( index == DOES_NOT_CONTAIN) // if not already holding ticker_
    {
        equities.push_back(Equity(ticker_));
        num_shares.push_back(quantity);
        return;
    }

    // if already holding ticker, add quantity of shares
    num_shares[index] += quantity;
}

void Portfolio::addEquity(const Equity &eq, int quantity)
{
    /*
    IMPORTANT: does not check validity of ticker_, only call in TwsApi callback to ensure ticker_ exists
    */
    int index = containsTicker(eq.getTicker());
    
    if( index == DOES_NOT_CONTAIN) // if not already holding ticker_
    {
        equities.push_back(eq);
        num_shares.push_back(quantity);
        return;
    }

    // if already holding ticker, add quantity of shares
    num_shares[index] += quantity;
}

void Portfolio::removeEquity(const std::string ticker_, int quantity)
{
    int index = containsTicker(ticker_);

    if( index == DOES_NOT_CONTAIN) // not holding ticker_
        return;
    
    num_shares[index] -= quantity;
}

int Portfolio::buyEquity(const std::string ticker_, const int num_shares_buy, const double price, const bool verbose)
{
    /*
    Conditions to buy:
    - must have enough money
    - ticker must exist (might check this when I call it in eclient and historical data)
    */

    double commission = getCommission(num_shares_buy);

    double cost = num_shares_buy * price + commission;

    if ( cash <= cost)
    {
        if ( verbose )
        {
            std::cout << std::endl << "---------- Purchase Details ----------" << std::endl;
            std::cout << "Insufficient Funds" << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }

        return INSUFFICIENT_FUNDS;
    }

    cash -= cost; // pay for stock + commission

    addEquity(ticker_, num_shares_buy);

    if( verbose )
    {
        std::cout << std::endl << "---------- Purchase Details ----------" << std::endl;
        std::cout << "Ticker: " << ticker_ 
                  << ", Number of Shares: " << num_shares_buy
                  << ", Gross Cost: " << (num_shares_buy * price) 
                  << ", Commission: " << commission 
                  << ", Total Cost: " << cost << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }

    return SUCCESSFUL_TRADE;
}

int Portfolio::buyEquity(const Equity &eq, const int num_shares_buy, const double price, const bool verbose)
{
    /*
    Conditions to buy:
    - must have enough money
    - ticker must exist (might check this when I call it in eclient and historical data)
    */

    double commission = getCommission(num_shares_buy);

    double cost = num_shares_buy * price + commission;

    if ( cash <= cost)
    {
        if ( verbose )
        {
            std::cout << std::endl << "---------- Purchase Details ----------" << std::endl;
            std::cout << "Insufficient Funds" << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }
        return INSUFFICIENT_FUNDS;
    }

    cash -= cost; // pay for stock + commission

    addEquity(eq, num_shares_buy);

    if( verbose )
    {
        std::cout << std::endl << "---------- Purchase Details ----------" << std::endl;
        std::cout << "Ticker: " << eq.getTicker() 
                  << ", Number of Shares: " << num_shares_buy
                  << ", Gross Cost: " << (num_shares_buy * price) 
                  << ", Commission: " << commission 
                  << ", Total Cost: " << cost << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }

    return SUCCESSFUL_TRADE;
}

int Portfolio::sellEquity(const std::string ticker_, const int num_shares_sell, const double price, const bool verbose)
{
    /*
    Conditions to sell:
    - must have enough shares
    - portfolio must contain equity with name ticker_
    */

    int index = containsTicker(ticker_);

    if( ( index != DOES_NOT_CONTAIN ) && ( num_shares[index] >= num_shares_sell ) ) // if ticker is in portfolio, and there are enough shares
    {

        int commission = getCommission(num_shares_sell);

        double proceeds = num_shares_sell*price - commission;

        cash += proceeds; // add cash from sale

        removeEquity(ticker_, num_shares_sell); // remove shares

        if( verbose )
        {
            std::cout << std::endl << "---------- Sale Details ----------" << std::endl;
            std::cout << "Ticker: " << ticker_ 
                    << ", Number of Shares: " << num_shares_sell
                    << ", Gross Proceeds: " << (num_shares_sell * price) 
                    << ", Commission: " << commission 
                    << " Net Proceeds: " << proceeds << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }
        return SUCCESSFUL_TRADE;
    }

    if ( verbose )
    {
        std::cout << std::endl << "---------- Sale Details ----------" << std::endl;
        std::cout << "Insufficient Shares, Number of Shares Held: " << num_shares[index]; // << ", Number of Shares Attempted to Sell: " << num_shares_sell << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }
        
    return INSUFFICIENT_SHARES;
}

} // end namespace