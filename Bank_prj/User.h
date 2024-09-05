#pragma once
#include <string>
#include <sqlite3.h>
#include "Account.h"
#include "TableNames.h"
#include "utilts.h"
#include <iostream>
#include <vector>
#include <map>
#include "Database.h"

class User
{
private:
	std::string _name;
	std::string _lastname;
	std::string _email;
	std::string  _password;
	unsigned _id;	
	const std::string path_to_database = "C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db";
public:
	User(std::string name, std::string ln, std::string em, std::string ps);
	User();
	bool loadAccount(std::string email);
	Account& createAccount(CurrencyName currency);
	unsigned getId();
	std::string getEmail() const;
	std::string getPassword() const;

	void setName     (const std::string name)     { _name = name; }
	void setLastname (const std::string lastname) { _lastname = lastname; }
	void setEmail    (const std::string email)    { _email = email; }
	void setPassword (const std::string password) { _password = password; }
	void setId       (const unsigned id)          { _id = id; }

	User& operator=(const User& other);

};

