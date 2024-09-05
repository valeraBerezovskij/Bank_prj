#include "User.h"

User::User(std::string name, std::string ln, std::string em, std::string ps) {
    Database db_user(path_to_database);

    std::map<std::string, std::string> user = {
        {"first_name", name},
        {"last_name", ln},
        {"email", em},
        {"password", ps}
    };

    if (!db_user.InsertData(TableName::Users, user)) {
        std::cerr << "Error inserting data to database." << std::endl;
        return;
    }

    this->_id = db_user.GetLastInsertId();
    this->_name = name;
    this->_lastname = ln;
    this->_email = em;
    this->_password = ps;

    std::cout << "\t" << _id << std::endl;
    std::cout << "\t" << _name << std::endl;
    std::cout << "\t" << _lastname << std::endl;
    std::cout << "\t" << _email << std::endl;
    std::cout << "\t" << _password << std::endl;
}

User::User()
{
}

bool User::loadAccount(std::string email) {
    std::cout << "loadAccounts START for email: " << email << std::endl;

    Database db_user(path_to_database);
    db_rows results;
    try {
        results = db_user.getResult("SELECT * FROM users WHERE email = '" + email + "';");
        std::cout << "Results received for: " << email << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }

    if (results.empty()) {
        std::cout << "No users found for user email: " << email << std::endl;
        return false;
    }
    else {
        for (const auto& row : results) {
            try {
                this->_id = std::stoi(row.at("user_id"));
                this->_name = row.at("first_name");
                this->_lastname = row.at("last_name");
                this->_email = row.at("email");
                this->_password = row.at("password");
                return true;
            }
            catch (const std::out_of_range& e) {
                std::cerr << "Error accessing data in row: " << e.what() << std::endl;
                return false;
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Error converting data in row: " << e.what() << std::endl;
                return false;
            }
        }
    }
    std::cout << "loadAccounts END." << std::endl;
    return true;
}

Account& User::createAccount(CurrencyName currency) {
    Account newAccount(_id, currency);
    return newAccount;
}

unsigned User::getId()
{
    return _id;
}

std::string User::getEmail() const
{
    return _email;
}

std::string User::getPassword() const
{
    return _password;
}

User& User::operator=(const User& other)
{
    if (this != &other) {

        this->_name = other._name;
        this->_lastname = other._lastname;
        this->_email = other._email;
        this->_password = other._password;
        this->_id = other._id;
    }
    return *this;
}
