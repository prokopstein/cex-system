#ifndef __EXCEPTIONS_H_
#define __EXCEPTIONS_H_

#include <exception>

// Here are defined all different types of exceptions raised in the app

// Limit (exchange, currency) pairs exceeded
class TooManySlots : public std::exception
{
public:
	TooManySlots() : std::exception()
	{
	}
};

// Internal data has been corrupted
class CorruptedTradingPairDictionary : public std::exception
{
public:
	CorruptedTradingPairDictionary() : std::exception()
	{
	}
};

// Trading slot or exchange not found
class NotFoundException : public std::exception
{
public:
	NotFoundException() : std::exception()
	{
	}
};

// Input data is not valid
class ValidationException : public std::exception
{
public:
	ValidationException() :
		message_("") { }

	ValidationException(const std::string& message) :
		message_(message) { }

	virtual char const *what() const noexcept { return message_.c_str(); }

private:
  std::string message_;
};

#endif
