#pragma once
#include <string>

bool usersExist();
void createUser(const std::string& login, const std::string& password);
bool checkLogin(const std::string& login, const std::string& password);
