#include <map>
#include <iostream>
#include "Bank.h"
#include "User.h"

typedef std::vector<std::pair<TableName, std::map<std::string, std::string>>> insert_data;

int main()
{
	Bank bank;
	const std::string path_to_database = "C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db";

	Database db(path_to_database);

    //std::cout << db.executeQuery("DROP TABLE users;");
	//std::cout << db.executeQuery("DROP TABLE accounts;");
	std::cout << "\n\n\n";
	auto res = db.getResult("SELECT * FROM users;");
	printResult(res);
	std::cout << "\n\nACCOUNTS:\n\n";
	res = db.getResult("SELECT * FROM accounts;");
	printResult(res);
	std::cout << "\n\n\n";

	db.createTable();
	bank.run();

	std::cout << "\n\n\n";
	res = db.getResult("SELECT * FROM users;");
	printResult(res);
	std::cout << "\n\nACCOUNTS:\n\n";
	res = db.getResult("SELECT * FROM accounts;");
	printResult(res);

	return 0;
}