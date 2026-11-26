#include "student.h"
#include <cctype>
#include <cstdio>
#include <cstring>

using namespace std;

static bool isDigit(char symbol) { return symbol >= '0' && symbol <= '9'; }

void clearStudent(Student& student) { memset(&student, 0, sizeof(student)); }

void setText(char* dest, size_t size, const string& source) {
    memset(dest, 0, size);
    strncpy(dest, source.c_str(), size - 1);
}

bool validateKey(const string& text, string& errorText) {
    if (text.empty() || text.size() >= KEY_LEN) {
        errorText = "номер зачётки должен содержать от 1 до 11 символов";
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (!isalnum(static_cast<unsigned char>(text[i]))) {
            errorText = "номер зачётки: допустимы только латинские буквы и цифры";
            return false;
        }
    }
    return true;
}

bool validateText(const string& text, size_t maxLen, const char* name, string& errorText) {
    if (text.empty() || text.size() >= maxLen) {
        errorText = string(name) + ": длина должна быть от 1 до " +
                    to_string(maxLen - 1) + " байт";
        return false;
    }
    return true;
}

bool parseGpa(const string& text, int32_t& result) {
    size_t i = 0;
    if (text.empty() || !isDigit(text[0])) return false;
    int32_t intPart = 0;
    while (i < text.size() && isDigit(text[i])) {
        intPart = intPart * 10 + (text[i] - '0');
        if (intPart > 9) return false;
        ++i;
    }
    int32_t fracPart = 0;
    int fracDigits = 0;
    if (i < text.size()) {
        if (text[i] != '.' && text[i] != ',') return false;
        ++i;
        while (i < text.size() && isDigit(text[i])) {
            if (fracDigits == 2) return false;
            fracPart = fracPart * 10 + (text[i] - '0');
            ++fracDigits;
            ++i;
        }
        if (i != text.size() || fracDigits == 0) return false;
    }
    if (fracDigits == 1) fracPart *= 10;
    result = intPart * 100 + fracPart;
    return result >= 200 && result <= 500;
}

bool isRecordValid(const Student& student) {
    if (memchr(student.zachetka, 0, KEY_LEN) == nullptr) return false;
    if (memchr(student.fio, 0, FIO_LEN) == nullptr) return false;
    if (memchr(student.group, 0, GROUP_LEN) == nullptr) return false;
    if (student.zachetka[0] == '\0' || student.fio[0] == '\0' || student.group[0] == '\0') return false;
    if (student.gpa100 < 200 || student.gpa100 > 500) return false;
    if (student.social > 1 || student.deleted > 1 || student.stipType > ST_INCREASED) return false;
    if (student.amount < 0 || student.amount > 1000000) return false;
    return true;
}

void calcStipend(Student& student) {
    if (student.gpa100 >= 500)      { student.stipType = ST_INCREASED; student.amount = 5000; }
    else if (student.gpa100 >= 400) { student.stipType = ST_ACADEMIC;  student.amount = 2500; }
    else if (student.social)        { student.stipType = ST_SOCIAL;    student.amount = 3500; }
    else                            { student.stipType = ST_NONE;      student.amount = 0; }
}

const char* stipName(uint8_t type) {
    switch (type) {
        case ST_SOCIAL:    return "социальная";
        case ST_ACADEMIC:  return "академическая";
        case ST_INCREASED: return "повышенная";
        default:           return "нет";
    }
}

void printHeader() {
    printf("%-12s %-30s %-10s %-6s %-14s %-6s\n",
                "Зачётка", "ФИО", "Группа", "Балл", "Стипендия", "Сумма");
}

void printStudent(const Student& student) {
    printf("%-12s %-30s %-10s %d.%02d  %-14s %-6d\n",
                student.zachetka, student.fio, student.group,
                student.gpa100 / 100, student.gpa100 % 100,
                stipName(student.stipType), student.amount);
}
