/*========================================================================
 * TwsApiC++ - Demonstration program
 *========================================================================
 */

// This code was put together in February 2023 and it has not been revised since. 
// This source code is meant to serve as a general example for how to use
// various EWrapper methods in the TWS API. This file combines some source code
// from examples on the JanBoonen repository which is why there is stylistic
// differences in some of the code. This file has only been formatted for
// readability using vim but nothing in the code has been changed so that it
// matches the code that was used in the demonstration video. 
//
// This source file shows a few different EWrapper methods which are not meant to be 
// used all at the same time. You will need to comment and uncomment the appropriate 
// things in the main function. 
//
// Not strictly necessary: 
// If you use vim text editor I recommend installing
// vim-commentary plugin: https://github.com/tpope/vim-commentary
//
// Video by VerrilloTrading (VT) that uses this source file for the demonstation: 
// https://youtu.be/5moyX0qwkCA

////////////////////////////////////////////////////////////////////////////////////

// Include the TWS C++ API Header file
#include "TwsApiL0.h"

///Faster: Check spelling of parameter at compile time instead of runtime.
#include "TwsApiDefs.h"

// Used for debugging and formatting text strings
// #include <boost/format.hpp>
// #include <algorithm>

using namespace TwsApi; // for TwsApiDefs.h

// The following macro is from JanBoonen source code to use a Sleep() function
// for debugging.  It makes sure that the Sleep() function works on Windows and
// Linux. I am not sure if it accounts for Mac. 

#ifdef WIN32
	#include <windows.h>		// Sleep(), in miliseconds
	#include <iostream>
	#include <process.h>
	#define CurrentThreadId GetCurrentThreadId
#else
	#include <unistd.h>			// usleep(), in microseconds
	#define Sleep( m ) usleep( m*1000 )
	#include <pthread.h>
	#define CurrentThreadId pthread_self
#endif

#define PrintProcessId printf("%ld  ", CurrentThreadId() )

// ----- USER DEFINED MACROS -----

// historical data formatting:
#define HIST_WHAT_TO_SHOW std::string("BID_ASK") // "FEE_RATE", "HISTORICAL_VOLATILITY", "OPTION_IMPLIED_VOLATILITY"

// field codes for live data:
#define DELAYED_BID 66
#define DELAYED_ASK 67
#define DELAYED_LAST 68
#define DELAYED_HIGH 72
#define DELAYED_LOW 73
#define DELAYED_CLOSE 75
#define DELAYED_OPEN 76

// ----- END USER DEFINED MACROS -----

// ----- USER DEFINED STRUCTS -----

struct MarketData {
    double bid = 0.0;
    double ask = 0.0;
    std::string time;
};

///Advantages of deriving from EWrapperL0
/// Faster: implement only the methods you need.
/// Safe: receive notification of methods called you didn't implement
/// Robust: exceptions thrown by your code are catched safely and notified to you
class YourEWrapper: public EWrapperL0 
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


//========================================================================
// main entry
//========================================================================
int main(void) 
{
	std::cout << "This is test in trading bot" << std::endl;
	MarketData md;
    ///Easier: just allocate your wrapper and instantiate the EClientL0 with it.
    YourEWrapper  YW( md, false );                      // false: not using the EReader
    EClientL0*    EC = EClientL0::New( &YW );

    if( EC->eConnect( "", 7496, 0 ) )
	{
		
        // ----- Start Contract -----
		Contract C;
		C.symbol = "SPY";
		C.secType = "STK";
		C.exchange = "SMART";
		C.currency = "USD";
        // ----- End Contract -----

        // ----- Start API Requests -----

		std::cout << "selecting delayed data..." << std::endl;
		EC->reqMarketDataType(3); // delayed

		Sleep(1000);
		
		std::cout << "retrieving delayed live data..." << std::endl;
		int tickerID = 1001; 
		EC->reqMktData ( tickerID , C , ""  , true ); 

		// std::cout << "retrieving historical data..." << std::endl;
		// EC->reqHistoricalData(4001, C, "20250101 00:00:00 UTC", "1 M", "1 day", HIST_WHAT_TO_SHOW, 1, 1);

        // ----- End API Requests -----

      	//	Easier: Call checkMessages() in a loop. No need to wait between two calls.
		while( YW.notDone() ) 
		{
			EC->checkMessages();
		
		// 	if(ReqOptions == 0) // Get number of strikes and call mkt data for those strikes. 
		// 	{
		// 		// sort vector from lowest to highest. 
		// 		// std::sort(YW.Strikes.begin(), YW.Strikes.end()); 					

		// 		// // Step 1: Calculate the absolute difference between the value and each element in the vector.
		// 		// for (const auto& strike : YW.Strikes)  					
		// 		// {
		// 		// 	double diff = std::abs(strike - UnderlyingPrice);
		// 		// 	differences.push_back(std::make_pair(diff, strike));
		// 		// }
				
		// 		// // sort by the difference value
		// 		// std::sort(differences.begin(), differences.end());

		// 		// // get the closest 20 values into a vector 
		// 		// std::vector<double> closest_numbers;
		// 		// for (int i = 0; i < 20; i++) 
		// 		// {
		// 		// 	closest_numbers.push_back(differences[i].second);
		// 		// } 
			
		// 		// Printing and Debugging  
		// 		/* std::cout<<boost::format("The 20 closest numbers to %f are: ")%UnderlyingPrice; */
		// 		/* for (const auto& number : closest_numbers) */ 
		// 		/* { */
		// 			/* std::cout << number << " "; // all on the same line. */ 
		// 		/* } */

		// 		/* std::cout << std::endl; // put a newline after it's done. */ 
				
		// 		// Print out the entire vector. 
		// 		/* for(int Index = 0; Index < YW.Strikes.size(); Index++) */ 
		// 		/* { */
		// 			/* std::cout << boost::format("Strike: %f \n")%YW.Strikes[Index]; */
		// 		/* } */   

		// 		// increment a ticker ID to request market data for each contract. 
		// 		// Ticker IDs need to be handled properly in TWS API Programs. 
		// 		// You should make sure every request for market data in
		// 		// your application has a unique ticker ID.
					

		// 		// EC->reqMarketDataType(3);
				
		// 		// EC->reqMktData ( tickerID , C , ""  , true ); 

		// 		// for(int Index = 0; Index < closest_numbers.size(); Index++)
		// 		// {
		// 		// 	// Getting strike prices from the vector. 
		// 		// 	C.strike = closest_numbers[Index]; 

		// 		// 	// Requesting market data for each contract. 
		// 		// 	EC->reqMktData ( tickerID , C , ""  , true ); 

		// 		// 	// increment the ticker ID
		// 		// 	tickerID++;
		// 		// } 

		// 		// Inform the program that we have made the request and it should not go through again
		// 		// int being used as a bool should just be a bool 
		// 		ReqOptions = 1; 		
				
		// 		// std::cout << "selecting delayed data..." << std::endl;
		// 		// EC->reqMarketDataType(3); // delayed

		// 		// std::cout << "retrieving delayed live data..." << std::endl;
		// 		// int tickerID = 1001; 
		// 		// EC->reqMktData ( tickerID , C , ""  , true ); 

		// 		// std::cout << "retrieving historical data..." << std::endl;
		// 		// EC->reqHistoricalData(4001, C, "20250101 00:00:00 UTC", "1 M", "1 day", HIST_WHAT_TO_SHOW, 1, 1);
		// 	} 

		// 	// Debug: Prints out how many contracts we retrieved. 
		// 	// std::cout << boost::format("vector size: %1% \n")%YW.Strikes.size();
		// 	// printf("all strikes retrieved, going to sleep\n"); 
		// 	Sleep(1000); // update interval for debugging
		}
    }

	std::cout << "ask: " << md.ask << std::endl;

    EC->eDisconnect();
    delete EC;
}
