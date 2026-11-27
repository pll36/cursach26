#include "auth.h"
#include "common.h"
#include "crypto.h"
#include "storage.h"
#include "student.h"
#include <cstring>
#include <fstream>
#include <random>

using namespace std;

struct UserRec {
    char     login[32];
    uint32_t salt;
    uint32_t hash;
};

static uint32_t hashPassword(const string& password, uint32_t salt) {
    uint32_t hash = fnv1a(&salt, sizeof(salt));
    hash = fnv1a(password.data(), password.size(), hash);
    for (int i = 0; i < 1000; ++i) hash = fnv1a(&hash, sizeof(hash), hash);
    return hash;
}

bool usersExist() { return fileExists(FILE_USERS); }

void createUser(const string& login, const string& password) {
    UserRec user;
    memset(&user, 0, sizeof(user));
    setText(user.login, sizeof(user.login), login);
    random_device randomDevice;
    user.salt = randomDevice();
    user.hash = hashPassword(password, user.salt);
    ofstream file(FILE_USERS, ios::binary | ios::trunc);
    if (!file) throw FileError("не удалось создать файл учётных записей");
    file.write(reinterpret_cast<const char*>(&user), sizeof(user));
    file.flush();
    if (!file) throw FileError("ошибка записи файла учётных записей");
}

bool checkLogin(const string& login, const string& password) {
    ifstream file(FILE_USERS, ios::binary);
    if (!file) throw FileError("не удалось открыть файл учётных записей");
    UserRec user;
    file.read(reinterpret_cast<char*>(&user), sizeof(user));
    if (!file) throw FileError("файл учётных записей повреждён");
    user.login[sizeof(user.login) - 1] = '\0';
    return login == user.login && hashPassword(password, user.salt) == user.hash;
}
