#pragma once
#include <string>
#include "currencies.h"
#include <iostream>
#include "Database.h"
#include "utilts.h"
#include <map>

class Account
{
private:
	unsigned _account_id;
	double _balance;
	std::string _card_code;
	CurrencyName _currency_code;
	const std::string path_to_database = "C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db";

public:

	Account(const unsigned user_id, const CurrencyName currency);
	Account();

	Account& getAccount(const unsigned id);
	void setAccountId(unsigned id) { _account_id = id; }
	void setBalance(double balance) { _balance = balance; }
	void setCardCode(const std::string& cardCode) { _card_code = cardCode; }
	void setCurrencyCode(const std::string& currencyCode) { _currency_code = StringToCurrencyName(currencyCode); }

	bool replenishBalance(unsigned id, double amount);
	bool withdrawMoney(unsigned id, double amount);
	bool sendMoney(unsigned sender_id, unsigned reciever_id, double amount);

	const void showInformation(unsigned id);

	unsigned getAccountId();
	CurrencyName getCurrency() const;

	Account& operator=(const Account& other);
};