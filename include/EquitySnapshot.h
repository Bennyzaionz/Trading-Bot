/*
This file defines the MarketSnapshot class which contains data about a stock price on a certain datetime
*/

#ifndef EQUITY_SNAPSHOT_h
#define EQUITY_SNAPSHOT_h

#include <string>

namespace AlgoTrading
{

class EquitySnapshot
{
    private:

        std::string datetime;
        double open;
        double close;
        double low;
        double high;
        double avg_bid;
        double avg_ask;
        double min_bid;
        double max_ask;
        int volume;

    public:

        /*---------- CONSTRUCTOR ----------*/
        EquitySnapshot
            (
                const std::string datetime_ = "",
                double open_ = -1,
                double close_ = -1,
                double low_ = -1,
                double high_ = -1,
                double avg_bid_ = -1,
                double avg_ask_ = -1,
                double min_bid_ = -1,
                double max_ask_ = -1,
                int volume_ = 0
            );

        /*---------- GETTERS ----------*/ 
        
        std::string getDatetime() const { return datetime; }
        double getOpen() const { return open; }
        double getClose() const { return close; }
        double getLow() const { return low; }
        double getHigh() const { return high; }
        double getAvgBid() const { return avg_bid; }
        double getAvgAsk() const { return avg_ask; }
        double getMinBid() const { return min_bid; }
        double getMaxAsk() const { return max_ask; }
        double getVolume() const { return volume; }
        
        /* ---------- SETTERS ---------*/

        void setOpen(double open_) { open = open_; }
        void setClose(double close_) { close = close_; }
        void setLow(double low_) { low = low_; }
        void setHigh(double high_) { high = high_; }
        void setAvgBid(double avg_bid_) { avg_bid = avg_bid_; }
        void setAvgAsk(double avg_ask_) { avg_ask = avg_ask_; }
        void setMinBid(double min_bid_) { min_bid = min_bid_; }
        void setMaxAsk(double max_ask_) { max_ask = max_ask_; }
        void setVolume(int volume_) { volume = volume_; }
        
        /*---------- PRINT HELPER ----------*/

        void print(const std::string print_type = "BID-ASK");
};

} // namespace

#endif // EQUITY_SNAPSHOT_H