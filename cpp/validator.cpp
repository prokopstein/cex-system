#include <sstream>
#include <iterator>
#include <iomanip>

#include "../h/validator.h"
#include "../h/exceptions.h"

using namespace std;

// get the timestamp from the input string
static time_t getTimestamp(const string& str)
{
	try
	{
		struct tm tm;
		istringstream iss(str);
		iss >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");

		time_t timestamp = mktime(&tm);
		if (timestamp == -1) throw ValidationException();

		return timestamp;
	}
	catch (...)
	{
		throw ValidationException("Invalid timestamp in update pair request");
	}
}

// get the double number from the input string
static double getDouble(const string& str)
{
	char *endptr;
	double res = strtod(str.c_str(), &endptr);

	if (endptr != str.c_str() + str.length()) throw ValidationException("Invalid number format");
	return res;
}

// parse input string whick represents the request
shared_ptr<Request> Validator::parseInput(const string& str) const
{
	// split by spaces
	istringstream iss(str);
	vector<string> parts{istream_iterator<string>{iss}, istream_iterator<string>{}};

	size_t size = parts.size();
	if (size < 1) throw ValidationException("Empty request");

	if (parts[0] == "STOP")
	{
		return shared_ptr<Request>(new StopRequest());
	}
	if (parts[0] == "EXCHANGE_RATE_REQUEST")
	{
		if (size != 5) throw ValidationException("Incorrect number parameters in EXCHANGE_RATE_REQUEST request");

		shared_ptr<ExchangeRequest> rq(new ExchangeRequest());
		rq->tsSrc = { parts[1], parts[2] };
		rq->tsDst = { parts[3], parts[4] };
		return rq;
	}
	if (size != 6) throw ValidationException("Unknown request");

	shared_ptr<UpdateRequest> rq(new UpdateRequest());
	rq->tpu.timestamp = getTimestamp(parts[0]);
	rq->tpu.exchange = parts[1];
	rq->tpu.src_currency = parts[2];
	rq->tpu.dst_currency = parts[3];
	rq->tpu.fw_factor = getDouble(parts[4]);
	rq->tpu.bw_factor = getDouble(parts[5]);

	if (rq->tpu.fw_factor * rq->tpu.bw_factor > 1.00001) throw ValidationException("Invalid fw/bw factor in update pair request");
	return rq;
}
