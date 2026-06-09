#include "user.h"
#include <iostream>
#include <iomanip>

user::user() : name(""), phoneNumber("") {}

user::user(std::string phone, std::string n)
    : phoneNumber(phone), name(n) {}

user::~user() {}

void user::In(std::ifstream& in) {
    in >> phoneNumber;
    // 用户名可能包含空格，使用 getline
    std::getline(in >> std::ws, name);
}

void user::print() const {
    std::cout << "  " << std::setfill(' ')
              << std::setw(8) << name
              << "  " << phoneNumber;
}

void user::SetPhone(std::string phone) { phoneNumber = phone; }
void user::SetName(std::string n)     { name = n; }

std::string user::GetPhone() const { return phoneNumber; }
std::string user::GetName()  const { return name; }
