#pragma once
#include<vector>
#include<unordered_map>
#include "Database.h"

void printResult(db_rows res);
double roundToPrecision(double number, int precision);
std::string generateCardCode();
void printTransactions(db_rows t);