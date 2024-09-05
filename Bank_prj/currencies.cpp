#include "currencies.h"

std::string CurrencyNameToString(CurrencyName currencyName) {
	switch (currencyName)
	{
	case CurrencyName::USD:
		return "USD";
		break;
	case CurrencyName::EUR:
		return "EUR";
		break;
	default:
		return "unknown";
		break;
	}
}

CurrencyName StringToCurrencyName(const std::string& currencyCode) {
	if (currencyCode == "USD") {
		return CurrencyName::USD;
	}
	else if (currencyCode == "EUR") {
		return CurrencyName::EUR;
	}
}