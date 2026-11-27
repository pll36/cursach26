#include "ops.h"
#include "common.h"
#include "storage.h"
#include <cstdio>
#include <cstring>
#include <fstream>

using namespace std;

int findByKey(const string& path, const string& key) {
    size_t count = countRecords(path);
    Student student;
    for (size_t i = 0; i < count; ++i) {
        readAt(path, i, student);
        if (!student.deleted && key == student.zachetka) return i;
    }
    return -1;
}

bool addRecord(const string& path, const Student& student) {
    if (fileExists(path) && findByKey(path, student.zachetka) >= 0) return false;
    appendRecord(path, student);
    return true;
}

void viewAll(const string& path) {
    size_t count = countRecords(path);
    size_t shown = 0;
    Student student;
    for (size_t i = 0; i < count; ++i) {
        readAt(path, i, student);
        if (student.deleted) continue;
        if (shown == 0) printHeader();
        printStudent(student);
        ++shown;
    }
    if (shown == 0) printf("Файл пуст.\n");
}

bool markDeleted(const string& path, const string& key) {
    int index = findByKey(path, key);
    if (index < 0) return false;
    Student student;
    readAt(path, index, student);
    student.deleted = 1;
    writeAt(path, index, student);
    return true;
}

size_t compactFile(const string& path) {
    size_t count = countRecords(path);
    ofstream tmpFile(FILE_TMP, ios::binary | ios::trunc);
    if (!tmpFile) throw FileError("не удалось создать временный файл");
    size_t removed = 0;
    Student student;
    try {
        for (size_t i = 0; i < count; ++i) {
            readAt(path, i, student);
            if (student.deleted) {
                ++removed;
                continue;
            }
            tmpFile.write(reinterpret_cast<const char*>(&student), sizeof(Student));
            if (!tmpFile) throw FileError("ошибка записи во временный файл");
        }
        tmpFile.close();
        if (!tmpFile) throw FileError("ошибка закрытия временного файла");
    } catch (...) {
        tmpFile.close();
        remove(FILE_TMP);
        throw;
    }
    replaceFile(FILE_TMP, path);
    return removed;
}

void calcAll(const string& path) {
    size_t count = countRecords(path);
    Student student;
    for (size_t i = 0; i < count; ++i) {
        readAt(path, i, student);
        if (student.deleted) continue;
        calcStipend(student);
        writeAt(path, i, student);
    }
}
