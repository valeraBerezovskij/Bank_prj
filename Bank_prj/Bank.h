#pragma once
#include <vector>
#include <string>
#include "Database.h"
#include "User.h"
#include "TableNames.h"
#include "utilts.h"
//#include "Account.h"

class Bank
{
private:
	const std::string path_to_database = "C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db";
	Database db;

	void registration(Account& acc);
	void login(Account& acc);
public:
	Bank();
	void run();

};
