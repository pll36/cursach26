#include "storage.h"
#include "common.h"
#include <cstdio>
#include <fstream>

using namespace std;

bool fileExists(const string& path) {
    ifstream file(path, ios::binary);
    return file.good();
}

void createEmpty(const string& path) {
    ofstream file(path, ios::binary | ios::trunc);
    if (!file) throw FileError("не удалось создать файл " + path);
}

size_t countRecords(const string& path) {
    ifstream file(path, ios::binary | ios::ate);
    if (!file) throw FileError(openErrorText(path));
    streamoff size = file.tellg();
    if (size < 0 || size % sizeof(Student) != 0)
        throw FileError("файл " + path + " повреждён (неверный размер)");
    return size / sizeof(Student);
}

void readAt(const string& path, size_t index, Student& student) {
    ifstream file(path, ios::binary);
    if (!file) throw FileError(openErrorText(path));
    file.seekg(index * sizeof(Student));
    file.read(reinterpret_cast<char*>(&student), sizeof(Student));
    if (!file) throw FileError("ошибка чтения файла " + path);
    if (!isRecordValid(student))
        throw FileError("запись №" + to_string(index + 1) + " в файле " + path +
                        " повреждена (недопустимые данные)");
}

void writeAt(const string& path, size_t index, const Student& student) {
    fstream file(path, ios::binary | ios::in | ios::out);
    if (!file) throw FileError(openErrorText(path));
    file.seekp(index * sizeof(Student));
    file.write(reinterpret_cast<const char*>(&student), sizeof(Student));
    file.flush();
    if (!file) throw FileError("ошибка записи в файл " + path);
}

void appendRecord(const string& path, const Student& student) {
    ofstream file(path, ios::binary | ios::app);
    if (!file) throw FileError(openErrorText(path));
    file.write(reinterpret_cast<const char*>(&student), sizeof(Student));
    file.flush();
    if (!file) throw FileError("ошибка записи в файл " + path);
}

// Основной файл заменяется только после полностью подготовленного временного (ТЗ 4.4.9)
void replaceFile(const string& tmpPath, const string& target) {
    string backup = target + ".bak";
    remove(backup.c_str());
    bool targetExists = fileExists(target);
    if (targetExists && rename(target.c_str(), backup.c_str()) != 0)
        throw FileError("не удалось подготовить замену файла " + target);
    if (rename(tmpPath.c_str(), target.c_str()) != 0) {
        if (targetExists) rename(backup.c_str(), target.c_str());
        throw FileError("не удалось заменить файл " + target + " (исходный сохранён)");
    }
    if (targetExists) remove(backup.c_str());
}
