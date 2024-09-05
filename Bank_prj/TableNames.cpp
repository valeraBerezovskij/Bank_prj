#include "TableNames.h"

std::string tableNameToString(TableName tableName) {
	switch (tableName)
	{
	case TableName::Users:
		return "users";
		break;
	case TableName::Accounts:
		return "accounts";
		break;
	case TableName::Currencies:
		return "currencies";
		break;
	case TableName::Transactions:
		return "transactions";
		break;
	default:
		return "unknown";
		break;
	}
}

