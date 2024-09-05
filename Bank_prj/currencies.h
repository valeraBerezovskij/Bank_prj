#pragma once
#include<string>

enum class CurrencyName {
    USD,
    EUR
};

std::string CurrencyNameToString(CurrencyName tableName);
CurrencyName StringToCurrencyName(const std::string& currencyCode);