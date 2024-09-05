#include "utilts.h"
#include <iostream>
#include <cmath>
#include <iomanip>

void printResult(db_rows res)
{
	for (size_t i = 0; i < res.size(); i++)
	{
		for (auto j : res[i]) {
			std::cout << j.first << ": " << j.second << std::endl;
		}
		std::cout << std::endl;
	}
}


double roundToPrecision(double number, int precision) {
	double factor = std::pow(10.0, precision);
	return std::round(number * factor) / factor;
}

std::string generateCardCode() {
    srand(static_cast<unsigned int>(time(0)));  // Seed the random number generator
    std::string cardCode;

    for (int i = 0; i < 16; ++i) {
        int digit = rand() % 10;  // Generate a random digit between 0 and 9
        cardCode += std::to_string(digit);

        // Add a space after every 4 digits, except after the last group
        if ((i + 1) % 4 == 0 && i != 15) {
            cardCode += " ";
        }
    }

    return cardCode;
}



void printTransactions(db_rows t) {
    // Печать заголовков с выравниванием
    std::cout << std::left
        << std::setw(15) << "Sender ID"
        << std::setw(15) << "Receiver ID"
        << std::setw(10) << "Amount"
        << std::setw(20) << "Transaction Type"
        << std::setw(10) << "Completed"
        << std::setw(20) << "Date"
        << std::endl;

    std::cout << std::string(105, '-') << std::endl;

    for (const auto& row : t) {
        std::string sender_id = row.at("sender_id");
        std::string receiver_id = row.at("receiver_id");
        std::string amount = row.at("amount");
        std::string type_name = row.at("type_name");
        std::string is_completed = row.at("is_completed") == "1" ? "Yes" : "No";
        std::string created_at = row.at("created_at");

        std::cout << std::left
            << std::setw(15) << sender_id
            << std::setw(15) << receiver_id
            << std::setw(10) << amount
            << std::setw(20) << type_name
            << std::setw(10) << is_completed
            << std::setw(20) << created_at
            << std::endl;
    }
}