#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <vector>
#include "Equity.h"

namespace AlgoTrading
{

const int DOES_NOT_CONTAIN = -1; // for containsTicker()

const int SUCCESSFUL_TRADE = 0;
const int INSUFFICIENT_FUNDS = 1;
const int INSUFFICIENT_SHARES = 2;

const double COMMISSION_PER_SHARE = 0.005; // from IBKR pro trading license: https://www.interactivebrokers.com/en/pricing/commissions-stocks.php
const int MIN_COMMISSION = 1; // also from IBKR pro trading license, any commission price less than 1 will round up to 1

class Portfolio
{
    private:
        
        double cash;
        std::vector<Equity> equities;
        std::vector<int> num_shares;

        double getCommission(int quantity) const;
        void addEquity(const std::string ticker_, const int quantity);
        void addEquity(const Equity &eq, const int quantity);
        void removeEquity(const std::string ticker_, int quantity);

    public:
        
        /*---------- CONSTRUCTOR ----------*/

        Portfolio(const double cash_ = -1); // only construct Portfolio with empty equities and num_shares to ensure that amounts line up

        /*---------- GETTERS ----------*/

        std::vector<std::string> getHoldings() const; // define in cpp
        std::vector<Equity> getEquities() const { return equities; }
        std::vector<int> getNumShares() const { return num_shares; }
        double getCash() const { return cash; }
        double getValue() const; // define in cpp
        int getSize() const { return equities.size(); }

        /*---------- PRINT HELPER ---------*/

        void print(int print_type = BID_ASK) const;

        /*---------- ADDING AND REMOVING EQUITIES ----------*/

        int containsTicker(const std::string ticker_ = "") const; // returns the index of the ticker if it exists, otherwise returns -1

        /*---------- BUYING AND SELLING ----------*/

        int buyEquity(const std::string ticker_, const int num_shares_buy, const double price, const bool verbose = false); // important to populate equity with data once the str is added
        int buyEquity(const Equity &eq, const int num_shares_buy, const double price, const bool verbose = false);
        int sellEquity(const std::string ticker_, const int num_shares_sell, const double price, const bool verbose = false);

};

} // end namespace

#endif