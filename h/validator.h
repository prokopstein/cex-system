#ifndef __VALIDATOR_H_
#define __VALIDATOR_H_

#include <memory>
#include "defs.h"

// Helper data structures and validator class to handle input data

// Base request
struct Request
{
	enum TYPE
	{
		RQ_EXCHANGE,
		RQ_UPDATE,
		RQ_STOP
	};

	Request(TYPE type) :
		rqType(type) { }

	virtual ~Request() { }

	TYPE rqType;
};

// Request to exchange
struct ExchangeRequest : public Request
{
	ExchangeRequest() :
		Request(RQ_EXCHANGE) { }

	TradingSlot tsSrc;
	TradingSlot tsDst;
};

// Request to update trading pair
struct UpdateRequest : public Request
{
	UpdateRequest() :
		Request(RQ_UPDATE) { }

	TradingPairUpdate tpu;
};

// Request to exit from app
struct StopRequest : public Request
{
	StopRequest() :
		Request(RQ_STOP) { }
};

class Validator
{
public:
	// Parse input string
	// Returns one of (ExchangeRequest, UpdateRequest, StopRequest) or raises Validation exception if input is not valid
	std::shared_ptr<Request> parseInput(const std::string& str) const;
};

#endif
