#include "Database.h"

Database::Database(const std::string& path) : _path(path), DB(nullptr)
{
	if (sqlite3_open(_path.c_str(), &DB)) {
		std::cerr << "Cant open database: " << sqlite3_errmsg(DB) << std::endl;
	}
	else {
		std::cout << "Database successfully opened." << std::endl;
	}
}

Database::~Database()
{
	if (DB) {
		sqlite3_close(DB);
	}
}

bool Database::executeQuery(const std::string& query) {
	char* errMsg = nullptr;
	if (sqlite3_exec(DB, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}
	return true;
}

bool Database::createTable()
{
	std::string createUsersTable = "CREATE TABLE IF NOT EXISTS users ("
		"user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"first_name TEXT NOT NULL, "
		"last_name TEXT NOT NULL, "
		"email TEXT NOT NULL UNIQUE, "
		"password TEXT NOT NULL, "
		"created_at DATETIME DEFAULT CURRENT_TIMESTAMP); ";

	std::string createTableCurrencies = "CREATE TABLE IF NOT EXISTS currencies ("
		"currency_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"currency_code TEXT NOT NULL UNIQUE,"
		"currency_name TEXT NOT NULL);";

	std::string createAccountsTable = "CREATE TABLE IF NOT EXISTS accounts ("
		"account_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"balance REAL DEFAULT 0.0, "
		"card_code TEXT NOT NULL UNIQUE, "
		"currency_code VARCHAR(3) NOT NULL, "
		"created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
		"FOREIGN KEY(currency_code) REFERENCES currencies(currency_code));";


	std::string createTableTransactions = "CREATE TABLE IF NOT EXISTS transactions ("
		"transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"sender_id INTEGER NOT NULL, "
		"receiver_id INTEGER NOT NULL, "
		"amount REAL NOT NULL, "
		"type_name TEXT NOT NULL, "
		"is_completed INTEGER NOT NULL CHECK (is_completed IN (0, 1)), "
		"created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
		"FOREIGN KEY(sender_id) REFERENCES accounts(account_id), "
		"FOREIGN KEY(receiver_id) REFERENCES accounts(account_id));";

	return (executeQuery(createTableTransactions) && executeQuery(createUsersTable)) &&
		   (executeQuery(createAccountsTable) && executeQuery(createTableCurrencies));
}

//Начало транзакции
bool Database::beginTransaction() {
	return executeQuery("BEGIN TRANSACTION;");
}

//Подтверждение транзакции
bool Database::commitTransaction() {
	return executeQuery("COMMIT;");
}

//Откат транзакции
bool Database::rollbackTransaction() {
	return executeQuery("ROLLBACK;");
}

bool Database::InsertData(TableName tableName, const std::map<std::string, std::string>& data) {
	std::string sql = "INSERT INTO " + std::string(tableNameToString(tableName)) + " (";
	std::string values = " VALUES (";

	for (auto it = data.begin(); it != data.end(); ++it) {
		sql += it->first;
		values += "?";
		if (std::next(it) != data.end()) {
			sql += ", ";
			values += ", ";
		}
	}

	sql += ")" + values + ");";

	sqlite3_stmt* stmt;
	//Создайте объект подготовленного оператора с помощью sqlite3_prepare_v2() .
	if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << std::endl;
		return false;
	}

	int index = 1;
	//Привяжите значения к параметрам с помощью интерфейсов sqlite3_bind_*().
	for (const auto& [key, value] : data) {
		sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_STATIC);
		++index;
	}

	//Запустите SQL, вызвав sqlite3_step() один или несколько раз.
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "Error executing statement: " << sqlite3_errmsg(DB) << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	//Уничтожьте объект с помощью sqlite3_finalize() .
	sqlite3_finalize(stmt);
	std::cout << "Data inserted successfully into " << tableNameToString(tableName) << " table." << std::endl;
	return true;
}	

bool Database::InsertMultipleTables(const insert_data& tableData) {
	if (!beginTransaction()) {
		return false;
	}

	for (const auto& [tableName, data] : tableData) {
		if (!InsertData(tableName, data)) {
			rollbackTransaction();
			return false;
		}
	}

	if (!commitTransaction()) {
		rollbackTransaction();
		return false;
	}

	return true;
}

db_rows Database::getResult(int id = -1, const std::string& columns = "*", TableName tableName)
{

	std::string _id, _table, _typeId;
	switch (tableName)
	{
	case TableName::Users:
		_typeId = "user_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Accounts:
		_typeId = "account_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Currencies:
		_typeId = "currency_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Transactions:
		_typeId = "transaction_id";
		_table = tableNameToString(tableName);
		break;
	default:
		std::cerr << "Unknown table." << std::endl;
		throw "Error";
		break;
	}

	std::string query;
	if (id == -1) {
		query = "SELECT " + columns + " FROM " + _table + ";";
	}
	else {
		if (!idExists(static_cast<unsigned>(id), tableName)) {
			throw std::runtime_error("ID does not exist.");
		}
		query = "SELECT " + columns + " FROM " + _table + " WHERE " + _typeId + " = " + _id + ";";
	}

	db_rows results;
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << std::endl;
		return results;
	}

	int colCount = sqlite3_column_count(stmt);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		std::unordered_map<std::string, std::string> row;
		for (int col = 0; col < colCount; ++col) {
			std::string colName = sqlite3_column_name(stmt, col);
			std::string colValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
			row[colName] = colValue;
		}
		results.push_back(row);
	}

	sqlite3_finalize(stmt);
	return results;
}

db_rows Database::getResult(const std::string query)
{
	db_rows results;
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << std::endl;
		return results;
	}

	int colCount = sqlite3_column_count(stmt);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		std::unordered_map<std::string, std::string> row;
		for (int col = 0; col < colCount; ++col) {
			std::string colName = sqlite3_column_name(stmt, col);
			std::string colValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
			row[colName] = colValue;
		}
		results.push_back(row);
	}

	sqlite3_finalize(stmt);
	return results;
}

bool Database::deleteData(const unsigned id, TableName tableName)
{
	if (!DB) {
		std::cerr << "Database not open." << std::endl;
		return false;
	}

	if (!idExists(id, tableName)) {
		std::cerr << "Id not exists." << std::endl;
		return false;
	}

	std::string _id, _table, _typeId;
	switch (tableName)
	{
	case TableName::Users:
		_typeId = "user_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Accounts:
		_typeId = "account_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Currencies:
		_typeId = "currency_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Transactions:
		_typeId = "transaction_id";
		_table = tableNameToString(tableName);
		break;
	default:
		std::cerr << "Unknown table." << std::endl;
		return false;
		break;
	}

	std::string sql = "DELETE FROM " + _table + " WHERE " + _typeId + " = " + _id + ";";
	char* errMsg = nullptr;

	if (sqlite3_exec(DB, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}
	else {
		std::cout << "Row deleted successfully" << std::endl;
		return true;
	}
	return false;
}

bool Database::idExists(const unsigned id, TableName tableName) {
	if (!DB) {
		std::cerr << "Database not open." << std::endl;
		return false;
	}
	std::string _id, _table, _typeId;
	switch (tableName)
	{
	case TableName::Users:
		_typeId = "user_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Accounts:
		_typeId = "account_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Currencies:
		_typeId = "currency_id";
		_table = tableNameToString(tableName);
		break;
	case TableName::Transactions:
		_typeId = "transaction_id";
		_table = tableNameToString(tableName);
		break;
	default:
		std::cerr << "Unknown table." << std::endl;
		return false;
		break;
	}
	const std::string id_str = std::to_string(id);
	const std::string query = "SELECT 1 FROM " + _table + " WHERE " + _typeId + " = " + id_str + " LIMIT 1;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << "id -- " << _id << std::endl;
		return false;
	}

	int result = sqlite3_step(stmt);
	bool exists = (result == SQLITE_ROW);

	sqlite3_finalize(stmt);
	return exists;
}

unsigned Database::GetId(const std::string& email)
{
	unsigned id;
	std::string sql = "SELECT user_id FROM users WHERE email = '" + email + "' LIMIT 1;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << std::endl;
		return -1;
	}

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		id = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
		return id;
	}
	else {
		std::cerr << "No data found for EMAIL: " << email << std::endl;
		sqlite3_finalize(stmt);
		return -1;
	}
	return id;
}

unsigned Database::GetAccountsCount(unsigned user_id)
{
	unsigned count = 0;
	std::string query = "SELECT COUNT(*) FROM accounts WHERE user_id = " + std::to_string(user_id) + ";";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			count = sqlite3_column_int(stmt, 0);
		}
	}
	else {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << std::endl;	
	}

	sqlite3_finalize(stmt);
	return count;
}

unsigned Database::GetLastInsertId() const {
	return static_cast<unsigned>(sqlite3_last_insert_rowid(DB));
}