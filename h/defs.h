#ifndef __DEFS_H_
#define __DEFS_H_

#include <string>
#include <vector>
#include <list>
#include <map>

#include <string.h>

// max number of (exchange, currency) pairs supported by the system
#define MAX_TRADING_SLOT_COUNT 1500

// exchange rate between exchanges for the same currency
#define SAME_CURRENCY_RATE	   1.0

// input data for trading pair update request
struct TradingPairUpdate
{
	std::string exchange;

	std::string src_currency;
	std::string dst_currency;

	double fw_factor;
	double bw_factor;

	time_t timestamp;
};

// trading slot entity (pair (exchange, currency))
struct TradingSlot
{
	std::string exchange;
	std::string currency;

	bool operator < (const TradingSlot& ts) const
	{
		return exchange + currency < ts.exchange + ts.currency;
	}
};

// trading slots storage, vector is fast to iterate and access by index
typedef std::vector<TradingSlot> TradingSlots;

// trading slots dictionary (quick search of slot id by (exchange, currency) pair)
typedef std::map<TradingSlot, size_t> TradingSlotDictionary;

// currency to trading slots dictionary (quick search of all trading slot ids' by the currency)
typedef std::map<std::string, std::list<size_t>> CurrencyToTradingSlotDictionary;

// trading pair entity ((exchange, currency) => (exchange, currency))
struct TradingPair
{
	size_t src;			// source trading slot index
	size_t dst;			// destination trading slot index

	double exrate;		// exchange rate between source and destination

	time_t tstamp;		// timestamp of the most actual update (the latest)
};

// key for trading pairs dictionary
struct TradingPairKey
{
	size_t src;			// source trading slot index
	size_t dst;			// destination trading slot index

	bool operator < (const TradingPairKey& tpk) const
	{
		return (src < tpk.src) || (src == tpk.src && dst < tpk.dst);
	}
};

// trading pairs storage, vector is fast to iterate and access by index
typedef std::vector<TradingPair> TradingPairs;

// trading pairs dictionary (quick serach for trading pair id by (source trading slot, destination trading slot) entity)
typedef std::map<TradingPairKey, size_t> TradingPairDictionary;

// helper structure to hold runtime calculated data for finding best rate
// can be used as two instances:
// 1) last calculated data - for immediate response
// 2) currenctly being calculated data in another thread - once it's done it becomes last calculated data
struct DataWorkspace
{
	std::vector<std::vector<double>> rate;
	std::vector<std::vector<size_t>> next;

	DataWorkspace(size_t dimension)
	{
		rate.resize(dimension);
		for (auto & v : rate)
		{
			v.resize(dimension, 0.0);
		}

		next.resize(dimension);
		for (auto & v : next)
		{
			v.resize(dimension, -1);
		}
	}

	void init()
	{
		for (auto & v : rate)
		{
			memset(v.data(), 0, v.size() * sizeof(double));
		}
		for (auto & v : next)
		{
			memset(v.data(), -1, v.size() * sizeof(size_t));
		}
	}
};

#endif
