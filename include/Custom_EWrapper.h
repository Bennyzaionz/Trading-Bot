#ifndef CUSTOM_EWRAPPER_H
#define CUSTOM_EWRAPPER_H

// Include the TWS C++ API Header file
#include "TwsApiL0.h"

///Faster: Check spelling of parameter at compile time instead of runtime.
#include "TwsApiDefs.h"

#include "Portfolio.h"

namespace AlgoTrading
{

class CustomEWrapper: public EWrapperL0 
{

    public:
        bool m_Done, m_ErrorForRequest;
        bool notDone( void ) { return !(m_Done || m_ErrorForRequest); }

  	MarketData& live_data;

 
  ///Easier: The EReader calls all methods automatically(optional)
  YourEWrapper( MarketData& md, bool runEReader = true ): EWrapperL0( runEReader ), live_data(md) {
    m_Done            = false;
    m_ErrorForRequest = false;
  }

  virtual void historicalData(TickerId reqId, const IBString& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps)
  {

	if( IsEndOfHistoricalData(date) )
	{
		std::cout << "Historical Data Finished" << std::endl << std::endl;

		m_Done = true;
	}

	else
	{
		if(HIST_WHAT_TO_SHOW == "BID_ASK")
		{
			std::cout << "Historical Data: \t"
					<< " - ReqID: " << reqId
					<< " - Date: " << date
					<< " - Time Average Bid: " << open
					<< " - Max Ask: " << high
					<< " - Min Bid: " << low
					<< " - Time Average Ask: " << close
					<< std::endl;
		}

		else
		{
			std::cout << "Historical Data: \t"
					<< " - ReqID: " << reqId
					<< " - Date: " << date
					<< " - Open: " << open
					<< " - High: " << high
					<< " - Low: " << low
					<< " - Close: " << close
					<< std::endl;			
		}
	}
  }

  virtual void tickPrice(TickerId tickerId, TickType field, double price, int canExecute) 
  {
	std::cout << "tick" << std::endl;
	
	if( field == DELAYED_BID )
	{
		std::cout << "Delayed Bid: " << price << std::endl; 
		live_data.bid = price;
	}

	else if( field == DELAYED_ASK )
	{
		std::cout << "Delayed Ask: " << price << std::endl;
		live_data.ask = price;
	}

	else if( field == DELAYED_LAST )
	{
		std::cout << "Delayed Last: " << price << std::endl;
	}

	else if( field == DELAYED_HIGH )
	{
		std::cout << "Delayed High: " << price << std::endl;
	}

	else if( field == DELAYED_LOW )
	{
		std::cout << "Delayed Low: " << price << std::endl;
	}

	else if( field == DELAYED_CLOSE )
	{
		std::cout << "Delayed Close: " << price << std::endl;
	}
	
	else if( field == DELAYED_OPEN )
	{
		std::cout << "Delayed Open: " << price << std::endl;
	}

	else
	{
		std::cout << "Other field " << field << ": " << price << std::endl;
	}
  }

  virtual void marketDataType(TickerId reqId, int marketDataType)
  {
	std::cout << "MarketDataType: " << marketDataType << std::endl << std::endl;

	m_Done = true;
  }

  ///Methods winError & error print the errors reported by IB TWS
  virtual void winError( const IBString& str, int lastError ) {
    fprintf( stderr, "WinError: %d = %s\n", lastError, (const char*)str );
    m_ErrorForRequest = true;
  }

  virtual void error( const int id, const int errorCode, const IBString errorString ) {
    fprintf( stderr, "Error for id=%d: %d = %s\n"
	   , id, errorCode, (const char*)errorString );
    m_ErrorForRequest = (id > 0);    // id == -1 are 'system' messages, not for user requests
  }
  
  virtual void connectionClosed()
	{
		PrintProcessId,printf( "Connection Closed\n");
	}

  ///Safer: uncatched exceptions are catched before they reach the IB library code.
  ///       The Id is tickerId, orderId, or reqId, or -1 when no id known
  virtual void OnCatch( const char* MethodName, const long Id ) {
    fprintf( stderr, "*** Catch in EWrapper::%s( Id=%ld, ...) \n", MethodName, Id );
  }

 	// class member variable to save the position quantity 
    int MyPosition = 0; 						

	// maybe use a vector or array to push back symbols along with position sizes to keep track of multiple positions. 
	IBString PositionSymbol = ""; 								  

	// Position updates
	virtual void position ( const IBString& account, const Contract& contract, int position, double avgCost) 	
	{
		// Do not include Forex Positions 
	 	if(contract.symbol != "USD" && contract.symbol != "EUR" && contract.symbol != "CAD") 		
		{
			MyPosition = position; // remember the position quantity. Append it to array 
			if(MyPosition != 0) // If there is a position, remember the symbol, append it to array. 
			{
				PositionSymbol = "AAPL"; // testing only one symbol for now. 
			}
			else
			{
			 	// if position closed, forget the symbol. 
				PositionSymbol = "";
			}

			std::cout <<boost::format("Position Update. Acct: %1% Symbol: %2% Position is: %3%\n")
			%account%PositionSymbol%MyPosition;
		} 
	}
	virtual void positionEnd()  
	{
		// did not finish testing this method
		
	}

  	// used to save contract details end response. 
  	int Req4 = 0; 	
	
	virtual void contractDetailsEnd(int reqId)
	{
		if(reqId == 4) // debug to confirm our request Id is valid. 
		{
			Req4 = 1; 
			printf("contract details end\n");
		}
	}
	
	IBString Expiry;
	std::vector<double>Strikes; 
  	virtual void contractDetails ( int reqId, const ContractDetails& contractDetails )
	{
		Contract C = contractDetails.summary;

		// Do not push back after we recieve the contract details end response. 
		if(C.strike != 0 && Req4 == 0) 		
		{
			Strikes.push_back(C.strike);
		}
	}

	// Used to get Options Live streaming data. 
    virtual void tickOptionComputation ( TickerId tickerId, TickType tickType, double impliedVol, 
	double delta, double optPrice, double pvDividend ,double gamma, double vega, double theta, double undPrice)
	{	
		std::cout << boost::format("TickerId: %1% TickType: %2% UndPrice: %3% ImpliedVol: %4% OptPrice: "
		"%5% Delta: %6% Gamma: %7% Vega: %8% Theta: %9% \n")
		%tickerId%*(TickTypes::ENUMS)tickType%undPrice%impliedVol%optPrice%delta%gamma%vega%theta; 
		
	}
	
	virtual void connectionOpened( void )
	{
		PrintProcessId,printf( "Connection Opened\n");
	}

	virtual void checkMessagesStarted( void )
	{
		PrintProcessId,printf( ">>> checkMessagesStarted\n");
	}

	virtual void checkMessagesStopped( void )
	{
		PrintProcessId,printf( "<<< checkMessagesStopped\n");
	}
};

} // end namespace

#endif