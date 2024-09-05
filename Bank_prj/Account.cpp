#include "Account.h"

Account::Account(const unsigned user_id, const CurrencyName currency)
	: _currency_code(currency), _balance(0.0) {

	Database db_user(path_to_database);
	std::string cardCode = generateCardCode();

	std::map<std::string, std::string> account = {
		{"card_code", cardCode},
		{"currency_code", CurrencyNameToString(currency)},
	};

	if (!db_user.InsertData(TableName::Accounts, account)) {
		std::cerr << "Error inserting data into the accounts table." << std::endl;
		return;
	}

	_account_id = user_id;
	_card_code = cardCode;

	std::cout << "Account created with ID: " << _account_id << std::endl;
}



Account::Account()
{

}

Account& Account::getAccount(const unsigned id)
{
	std::cout << "getAccount START for id: " << id << std::endl;

	Database db_user(path_to_database);
	db_rows results;
	try {
		results = db_user.getResult("SELECT * FROM accounts WHERE account_id = " + std::to_string(id) + ";");
		std::cout << "Results received for: " << id << std::endl;
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	if (results.empty()) {
		std::cout << "No accounts found for account id: " << id << std::endl;
	}
	else {
		for (const auto& row : results) {
			try {
				Account acc;

				acc.setAccountId(std::stoi(row.at("account_id")));
				acc.setBalance(std::stod(row.at("balance")));
				acc.setCardCode(row.at("card_code"));
				acc.setCurrencyCode(row.at("currency_code"));

				return acc;
			}
			catch (const std::out_of_range& e) {
				std::cerr << "Error accessing data in row: " << e.what() << std::endl;
			}
			catch (const std::invalid_argument& e) {
				std::cerr << "Error converting data in row: " << e.what() << std::endl;
			}
		}
	}
	std::cout << "getAccount END." << std::endl;

}

bool Account::replenishBalance(unsigned id, double amount)
{
	Database db("C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db");
	std::string sql = "UPDATE accounts SET balance = balance + '" + std::to_string(roundToPrecision(amount, 2)) + "' WHERE account_id = " + std::to_string(id) + ";";

	if (db.executeQuery(sql)) {
		std::map<std::string, std::string> res = {
			{"sender_id", std::to_string(id)},
			{"receiver_id", std::to_string(id)},
			{"amount",  std::to_string(amount)},
			{"type_name", "replenishment"},
			{"is_completed", "1"}
		};
		db.InsertData(TableName::Transactions, res);
		return true;
	}
	else {
		std::map<std::string, std::string> res = {
			{"sender_id", std::to_string(id)},
			{"receiver_id", std::to_string(id)},
			{"amount",  std::to_string(amount)},
			{"type_name", "replenishment"},
			{"is_completed", "0"}
		};
		db.InsertData(TableName::Transactions, res);
		return false;
	}
}

bool Account::withdrawMoney(unsigned id, double amount)
{
	Database db("C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db");

	db_rows rec = db.getResult("SELECT balance FROM accounts WHERE account_id = " + std::to_string(id) + ";");
	std::string sql = "UPDATE accounts SET balance = balance - '" + std::to_string(roundToPrecision(amount, 2)) + "' WHERE account_id = " + std::to_string(id) + ";";

	if (!rec.empty()) {
		std::string balanceStr = rec[0]["balance"];
		double balance = std::stod(balanceStr);

		if (amount < balance) {
			if (db.executeQuery(sql)) {
				std::map<std::string, std::string> res = {
					{"sender_id", std::to_string(id)},
					{"receiver_id", std::to_string(id)},
					{"amount",  std::to_string(amount)},
					{"type_name", "withdrawal"},
					{"is_completed", "1"}
				};
				db.InsertData(TableName::Transactions, res);
				return true;
			}
			else { return false; }
		}
		else {
			std::map<std::string, std::string> res = {
				{"sender_id", std::to_string(id)},
				{"receiver_id", std::to_string(id)},
				{"amount",  std::to_string(amount)},
				{"type_name", "withdrawal"},
				{"is_completed", "0"}
			};
			db.InsertData(TableName::Transactions, res);
			std::cout << "Not enought balance." << std::endl;
			return false;
		}
	}
	else {
		std::cout << "No data found for account id " << id << std::endl;
		return false;
	}
}

bool Account::sendMoney(unsigned sender_id, unsigned reciever_id, double amount)
{
	Database db("C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db");

	db_rows rec = db.getResult("SELECT balance FROM accounts WHERE account_id = " + std::to_string(sender_id) + ";");
	std::string balanceStr = rec[0]["balance"];
	double balance = std::stod(balanceStr);

	if (amount > balance) {
		std::cerr << "Not enough money." << std::endl;
		return false;
	}
	if (!db.idExists(sender_id, TableName::Accounts)) {
		std::cerr << "No such sender id." << std::endl;
		return false;
	}

	if (!db.idExists(reciever_id, TableName::Accounts)) {
		std::cerr << "No such reciever id." << std::endl;
		return false;
	}

	if (!db.beginTransaction()) {
		return false;
	}

	std::string sql_repl = "UPDATE accounts SET balance = balance + '"
		+ std::to_string(roundToPrecision(amount, 2)) + "' WHERE account_id = "
		+ std::to_string(reciever_id) + ";";
	std::string sql_withdr = "UPDATE accounts SET balance = balance - '"
		+ std::to_string(roundToPrecision(amount, 2)) + "' WHERE account_id = "
		+ std::to_string(sender_id) + ";";

	if (db.executeQuery(sql_withdr)) {
		if (!db.executeQuery(sql_repl)) {
			db.rollbackTransaction();
			std::cout << "Error with execute transactions." << std::endl;
		}
	}
	else {
		db.rollbackTransaction();
		std::cout << "Error with execute transactions." << std::endl;
	}

	if (db.commitTransaction()) {
		std::map<std::string, std::string> res = {
				{"sender_id", std::to_string(sender_id)},
				{"receiver_id", std::to_string(reciever_id)},
				{"amount",  std::to_string(amount)},
				{"type_name", "transfer"},
				{"is_completed", "1"}
		};
		if (!db.InsertData(TableName::Transactions, res)) {
			std::cout << "Error with insert data to transaction table." << std::endl;
		}
		return true;
	}
	else {
		db.rollbackTransaction();
		std::cout << "Error with commit transaction." << std::endl;
		std::map<std::string, std::string> res = {
		{"sender_id", std::to_string(sender_id)},
		{"receiver_id", std::to_string(reciever_id)},
		{"amount",  std::to_string(amount)},
		{"type_name", "transfer"},
		{"is_completed", "0"}
		};
		if (!db.InsertData(TableName::Transactions, res)) {
			std::cout << "Error with insert data to transaction table." << std::endl;
		}
		return false;
	}
}

const void Account::showInformation(unsigned id)
{
	Database db("C:\\Users\\Admin\\source\\repos\\Bank_prj\\Bank_prj\\SQLite\\bank_prj.db");
	if (!db.idExists(id, TableName::Accounts)) {
		std::cerr << "No such id." << std::endl;
		return;
	}
	const std::string query = "SELECT * FROM accounts WHERE account_id = " + std::to_string(id) + ";";
	auto print = db.getResult(query);
	printResult(print);
}

unsigned Account::getAccountId()
{
	std::cout << "\treturn accID Start." << std::endl;
	return _account_id;
}

CurrencyName Account::getCurrency() const {
	return _currency_code;
}

Account& Account::operator=(const Account& other) {
	if (this != &other) {

		_account_id = other._account_id;
		_balance = other._balance;
		_card_code = other._card_code;
		_currency_code = other._currency_code;
	}
	return *this;
}
