#pragma once
#include <cerrno>
#include <stdexcept>
#include <string>

// Исключение для ошибок файловых операций
class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& msg) : std::runtime_error(msg) {}
};

// Причина неудачного открытия файла (вызывать сразу после неудачи, пока errno не изменён)
inline std::string openErrorText(const std::string& path) {
    if (errno == ENOENT) return "файл " + path + " не найден";
    if (errno == EACCES || errno == EPERM) return "отказано в доступе к файлу " + path;
    return "не удалось открыть файл " + path;
}

// Имена файлов заданы константами: пользователь путь не вводит,
// поэтому доступ к произвольным файлам системы исключён (ТЗ 4.4.8).
const char* const FILE_STUDENTS  = "students.bin";   // исходный файл
const char* const FILE_TMP       = "students.tmp";   // временный файл
const char* const FILE_STATEMENT = "statement.bin";  // второй файл (ведомость)
const char* const FILE_SIGNATURE = "statement.sig";  // подпись ведомости
const char* const FILE_USERS     = "users.bin";      // учётная запись
