#ifndef __EXCHANGE_H_
#define __EXCHANGE_H_

#include <memory>
#include "defs.h"

// Trading system interface

class CurrencyExchange
{
public:
	virtual ~CurrencyExchange() { }

	// Update trading pair from the input
	virtual void UpdateTradingPair(const TradingPairUpdate& tpu) = 0;

	// Obtain the best exchange rate for the pair
	// Returns best rate value, populates input param 'path'
	virtual double GetBestRate(const TradingSlot& src, const TradingSlot& dst, std::list<TradingSlot>& path) = 0;

	// Get the instance of the system
	static std::shared_ptr<CurrencyExchange> Instance();
};

#endif
