#include "Bank.h"

Bank::Bank() : db(path_to_database)
{
	//TODO
}

void Bank::run()
{
    bool exit = false;
	char responce;
	Account acc;
	std::cout << "Log In or Register? (l/r)";
	if (std::cin >> responce; responce == 'l') {
		login(acc);
	}
	else if (responce == 'r') {
		registration(acc);
	}
	else {
		std::cerr << "Incorrect input." << std::endl;
		return;
	}

    int ch;
    while (!exit) {

        std::cout << "1 - Replenish balance" << std::endl;
        std::cout << "2 - Withdraw money" << std::endl;
        std::cout << "3 - Send money" << std::endl;
        std::cout << "4 - Show information about my account" << std::endl;
        std::cout << "5 - Show last 5 transactions" << std::endl;
        std::cout << "0 - Exit" << std::endl;
        std::cin >> ch;
        db_rows res;
        unsigned amount;
        switch (ch)
        {
        case 0:
            exit = true;
            break;
        case 1:
            std::cout << "Enter amount: ";
            std::cin >> amount;
            if (acc.replenishBalance(acc.getAccountId(), amount)) {
                std::cout << "Confirmed." << std::endl;
            }
            else {
                std::cout << "Not confirmed." << std::endl;
            }
            break;
        case 2:
            std::cout << "Enter amount: ";
            std::cin >> amount;
            if (acc.withdrawMoney(acc.getAccountId(), amount)) {
                std::cout << "Confirmed." << std::endl;
            }
            else {
                std::cout << "Not confirmed." << std::endl;
            }
            break;
        case 3:
            unsigned reciever_id;
            std::cout << "Enter user id: ";
            std::cin >> reciever_id;
            std::cout << "Enter amount: ";
            std::cin >> amount;
            if (acc.sendMoney(acc.getAccountId(), reciever_id, amount)) {
                std::cout << "Confirmed." << std::endl;
            }
            else {
                std::cout << "Not confirmed." << std::endl;
            }
            break;
        case 4:
            acc.showInformation(acc.getAccountId());
            break;
        case 5:
            res = db.getResult("SELECT * FROM transactions WHERE sender_id = " + std::to_string(acc.getAccountId()) + " ORDER BY created_at DESC LIMIT 5;");
            printTransactions(res);
            break;
        default:
            std::cerr << "No such choice." << std::endl;
            break;
        }
    }
    
}

void Bank::registration(Account& acc) {
    std::string name, lastname, email, password, currency;

    std::cout << "Enter your name: ";
    std::cin >> name;

    std::cout << "Enter your lastname: ";
    std::cin >> lastname;

    std::cout << "Enter your email: ";
    std::cin >> email;

    std::cout << "Enter your password: ";
    std::cin >> password;

    std::cout << "Choose currency (USD/EUR): ";
    std::cin >> currency;

    while (currency != "USD" && currency != "EUR") {
        std::cerr << "Invalid currency. Please choose either USD or EUR: ";
        std::cin >> currency;
    }
    CurrencyName currencyType = (currency == "USD") ? CurrencyName::USD : CurrencyName::EUR;

    User user(name, lastname, email, password);
    acc = user.createAccount(currencyType);
}

void Bank::login(Account& acc)
{
    User user;
    std::string email, password;
    std::cout << "Enter your email: ";
    std::cin >> email;
    while (!user.loadAccount(email)) {
        std::cin >> email;
    }
    
    std::cout << "Enter your password: ";
    std::cin >> password;
    if (user.getPassword() != password) {
        std::cerr << "Incorrect password. Retry: ";
    }
    while (user.getPassword() != password) {
        std::cin >> password;
        std::cerr << "Incorrect password. Retry: ";
    }

    acc = acc.getAccount(user.getId());
}