#pragma once
#include<string>

enum class TableName {
    Users,
    Accounts,
    Currencies,
    Transactions
};

std::string tableNameToString(TableName tableName);