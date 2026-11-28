#include "statement.h"
#include "common.h"
#include "crypto.h"
#include "storage.h"
#include "student.h"
#include <fstream>

using namespace std;

size_t formStatement(const string& source, const string& dest, const string& group) {
    size_t count = countRecords(source);
    ofstream outFile(dest, ios::binary | ios::trunc);
    if (!outFile) throw FileError("не удалось создать файл " + dest);
    size_t selected = 0;
    Student student;
    for (size_t i = 0; i < count; ++i) {
        readAt(source, i, student);
        if (student.deleted || student.stipType == ST_NONE) continue;
        if (!group.empty() && group != student.group) continue;
        outFile.write(reinterpret_cast<const char*>(&student), sizeof(Student));
        if (!outFile) throw FileError("ошибка записи файла " + dest);
        ++selected;
    }
    outFile.close();
    if (!outFile) throw FileError("ошибка закрытия файла " + dest);
    return selected;
}

void signStatement(const string& stmtPath, const string& sigPath, SignInfo& info) {
    info.hash    = hashFile(stmtPath);
    info.hashMod = info.hash % RSA_N;
    info.sig     = rsaSign(info.hash);
    info.valid   = true;
    ofstream file(sigPath, ios::trunc);
    if (!file) throw FileError("не удалось создать файл подписи");
    file << info.hash << ' ' << info.sig << '\n';
    file.flush();
    if (!file) throw FileError("ошибка записи файла подписи");
}

void verifyStatement(const string& stmtPath, const string& sigPath, SignInfo& info) {
    ifstream file(sigPath);
    if (!file) throw FileError(openErrorText(sigPath));
    uint32_t savedHash = 0, signature = 0;
    if (!(file >> savedHash >> signature)) throw FileError("файл подписи повреждён");
    info.hash    = hashFile(stmtPath);
    info.hashMod = info.hash % RSA_N;
    info.sig     = signature;
    info.valid   = rsaVerify(info.hash, signature);
}

void tamperStatement(const string& stmtPath) {
    fstream file(stmtPath, ios::binary | ios::in | ios::out);
    if (!file) throw FileError("не удалось открыть файл " + stmtPath);
    char symbol = 0;
    file.seekg(0);
    file.get(symbol);
    if (!file) throw FileError("ведомость пуста");
    symbol = symbol ^ 0x01;
    file.seekp(0);
    file.put(symbol);
    file.flush();
    if (!file) throw FileError("ошибка записи файла " + stmtPath);
}
