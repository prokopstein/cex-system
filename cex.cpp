#include <iostream>

#include "h/exchange.h"
#include "h/exceptions.h"
#include "h/validator.h"

using namespace std;

// TODO: unit tests
// TODO: more testing data

static void print_path(const list<TradingSlot>& path)
{
	for (auto el : path)
	{
		cout << el.exchange << ", " << el.currency << endl;
	}
}

static void update(shared_ptr<Request> rq)
{
	try
	{
		shared_ptr<UpdateRequest> urq = dynamic_pointer_cast<UpdateRequest>(rq);
		CurrencyExchange::Instance()->UpdateTradingPair(urq->tpu);
	}
	catch (TooManySlots&)
	{
		cout << "Too many trading slots" << endl;
	}
	catch (CorruptedTradingPairDictionary&)
	{
		cout << "Unrecovered system error" << endl;
	}
}

static void exchange(shared_ptr<Request> rq)
{
	try
	{
		shared_ptr<ExchangeRequest> erq = dynamic_pointer_cast<ExchangeRequest>(rq);

		list<TradingSlot> path;
		double rate = CurrencyExchange::Instance()->GetBestRate(erq->tsSrc, erq->tsDst, path);

		cout << endl << "BEST_RATES_BEGIN " << erq->tsSrc.exchange << " " << erq->tsSrc.currency << " " << erq->tsDst.exchange << " " << erq->tsDst.currency << " " << rate << endl;
		print_path(path);
		cout << "BEST_RATES_END" << endl;
	}
	catch (NotFoundException&)
	{
		cout << "Couldn't find exchange rate" << endl;
	}
}

int main(int argc, char* argv[])
{
	cout << "Type STOP to exit" << endl << endl;

	Validator validator;

	string str;

	try
	{
		while (getline(cin, str))
		{
			try
			{
				shared_ptr<Request> rq = validator.parseInput(str);

				if (rq->rqType == Request::RQ_STOP) break;
				if (rq->rqType == Request::RQ_UPDATE) update(rq);
				if (rq->rqType == Request::RQ_EXCHANGE) exchange(rq);
			}
			catch (ValidationException& e)
			{
				cout << "[warning] " << e.what() << endl;
			}
		}
	}
	catch (...)
	{
		cout << "Unknown system error, the app will stop..." << endl;
	}
	return 0;
}
