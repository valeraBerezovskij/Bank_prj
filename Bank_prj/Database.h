#pragma once
#include<sqlite3.h>
#include<string>
#include<unordered_map>
#include<vector>
#include<map>
#include <iostream>
#include "TableNames.h"

typedef std::vector<std::unordered_map<std::string, std::string>> db_rows;
typedef std::vector<std::pair<TableName, std::map<std::string, std::string>>> insert_data;

class Database
{
private:
	sqlite3* DB;
	std::string _path;
public:
	Database(const std::string& path);
	~Database();

	bool     idExists    (const unsigned id, TableName tableName);
	unsigned GetId       (const std::string& email);
	bool     executeQuery(const std::string& query);
	bool     createTable ();
	db_rows  getResult   (int id, const std::string& column, TableName tableName = TableName::Users);
	db_rows  getResult   (const std::string query);
	bool     deleteData  (const unsigned id, TableName tableName);

	bool beginTransaction();
	bool commitTransaction();
	bool rollbackTransaction();

	bool InsertData(TableName tableName, const std::map<std::string, std::string>& data);
	bool InsertMultipleTables(const insert_data& tableData);
	unsigned GetAccountsCount(unsigned user_id);
	unsigned GetLastInsertId() const;
};
