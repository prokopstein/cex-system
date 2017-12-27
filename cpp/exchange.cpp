#include "../h/exchange.h"
#include "../h/exceptions.h"

using namespace std;

class CurrencyExchangeImpl : public CurrencyExchange
{
	size_t maxTradingSlotCount_;
	bool needToRecalculate_;

	DataWorkspace dataWorkspace_;

	TradingSlots tradingSlots_;
	TradingSlotDictionary tradingSlotDictionary_;
	CurrencyToTradingSlotDictionary currencyToTradingSlotDictionary_;

	TradingPairs tradingPairs_;
	TradingPairDictionary tradingPairDictionary_;

public:
	CurrencyExchangeImpl(size_t maxTradingSlotCount) :
		maxTradingSlotCount_(maxTradingSlotCount),
		needToRecalculate_(true),
		dataWorkspace_(maxTradingSlotCount)
	{
	}

	~CurrencyExchangeImpl()
	{
	}

	void UpdateTradingPair(const TradingPairUpdate& tpu)
	{
		TradingSlot tsSrc = { tpu.exchange, tpu.src_currency }, tsDst = { tpu.exchange, tpu.dst_currency };

		// find input slot ids'
		size_t tsSrcId = GetAddSlot(tsSrc), tsDstId = GetAddSlot(tsDst);

		TradingPair tpSrc = { tsSrcId, tsDstId, 0, 0 }, tpDst = { tsDstId, tsSrcId, 0, 0 };

		// find trading pair ids'
		size_t tpSrcId = GetAddPair(tpSrc), tpDstId = GetAddPair(tpDst);

		TradingPair& pairSrc = tradingPairs_[tpSrcId];
		TradingPair& pairDst = tradingPairs_[tpDstId];

		// update pairs
		if (tpu.timestamp >= pairSrc.tstamp)
		{
			pairSrc.exrate = tpu.fw_factor;
			pairSrc.tstamp = tpu.timestamp;
		}

		if (tpu.timestamp >= pairDst.tstamp)
		{
			pairDst.exrate = tpu.bw_factor;
			pairDst.tstamp = tpu.timestamp;
		}

		// need to recalculate working data
		needToRecalculate_ = true;
	}

	double GetBestRate(const TradingSlot& src, const TradingSlot& dst, list<TradingSlot>& path)//const
	{
		// find trading slot ids'
		size_t srcIndex = GetSlotIndex(src), dstIndex = GetSlotIndex(dst);

		CalculateBestRates();

		if (dataWorkspace_.next[srcIndex][dstIndex] == (size_t)-1)
			throw NotFoundException();

		double rate = 1.0;
		path.push_back(tradingSlots_[srcIndex]);

		while (srcIndex != dstIndex)
		{
			// get next
			size_t nextIndex = dataWorkspace_.next[srcIndex][dstIndex];

			// claculate rate
			TradingPairKey tpk = { srcIndex, nextIndex };
			rate *= tradingPairs_[GetPairIndex(tpk)].exrate;

			// go to next
			srcIndex = nextIndex;

			// save path
			path.push_back(tradingSlots_[srcIndex]);
		}
		return rate;
	}

	void CalculateBestRates()
	{
		// just get out if no need to calculate working data
		if (!needToRecalculate_) return;

		// reset working data
		dataWorkspace_.init();

		for (auto tp : tradingPairs_)
		{
			dataWorkspace_.rate[tp.src][tp.dst] = tp.exrate;
			dataWorkspace_.next[tp.src][tp.dst] = tp.dst;
		}

		size_t tradingPairsCount = tradingPairs_.size();
		for (size_t k = 0; k < tradingPairsCount; ++k)
		{
			for (size_t i = 0; i < tradingPairsCount; ++i)
			{
				for (size_t j = 0; j < tradingPairsCount; ++j)
				{
					if (dataWorkspace_.rate[i][j] < dataWorkspace_.rate[i][k] * dataWorkspace_.rate[k][j])
					{
						dataWorkspace_.rate[i][j] = dataWorkspace_.rate[i][k] * dataWorkspace_.rate[k][j];
						dataWorkspace_.next[i][j] = dataWorkspace_.next[i][k];
					}
				}
			}
		}

		// update flag
		needToRecalculate_ = false;
	}

	// returns slot index
	// creates a new one if it doesn't exist
	// create all links: (same currency => all exchanges)
	size_t GetAddSlot(const TradingSlot& ts)
	{
		auto tsIt = tradingSlotDictionary_.find(ts);
		if (tsIt != tradingSlotDictionary_.end()) return tsIt->second;

		size_t newSlotId = tradingSlots_.size();
		if (newSlotId >= maxTradingSlotCount_) throw TooManySlots();

		tradingSlots_.push_back(ts);
		tradingSlotDictionary_[ts] = newSlotId;

		list<size_t>& exList = currencyToTradingSlotDictionary_[ts.currency];
		for (auto id : exList)
		{
			TradingPair src = { newSlotId, id, SAME_CURRENCY_RATE, 0 };
			GetAddPair(src, true);

			TradingPair dst = { id, newSlotId, SAME_CURRENCY_RATE, 0 };
			GetAddPair(dst, true);
		}

		exList.push_back(newSlotId);
		return newSlotId;
	}

	// returns trading pair index
	// creates a new one if it doesn't exist
	// if addOnly flag is true then creation only is allowed (not retrieval of existing one)
	size_t GetAddPair(const TradingPair& tp, bool addOnly = false)
	{
		TradingPairKey tpk = { tp.src, tp.dst };

		auto it = tradingPairDictionary_.find(tpk);
		if (it != tradingPairDictionary_.end())
		{
			if (addOnly) throw CorruptedTradingPairDictionary();
			return it->second;
		}

		size_t newPairId = tradingPairs_.size();
		tradingPairs_.push_back(tp);

		tradingPairDictionary_[tpk] = newPairId;
		return newPairId;
	}

	// get index or raise esception
	size_t GetSlotIndex(const TradingSlot& ts)
	{
		auto it = tradingSlotDictionary_.find(ts);
		if (it == tradingSlotDictionary_.end()) throw NotFoundException();

		return it->second;
	}

	// get index or raise esception
	size_t GetPairIndex(const TradingPairKey& tpk)
	{
		auto it = tradingPairDictionary_.find(tpk);
		if (it == tradingPairDictionary_.end()) throw NotFoundException();

		return it->second;
	}
};

// getting the instance of CurrencyExchange (which is singleton)
shared_ptr<CurrencyExchange> CurrencyExchange::Instance()
{
	static shared_ptr<CurrencyExchange> instance(new CurrencyExchangeImpl(MAX_TRADING_SLOT_COUNT));
	return instance;
}
