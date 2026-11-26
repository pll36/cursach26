#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

const size_t KEY_LEN   = 12;
const size_t FIO_LEN   = 96;
const size_t GROUP_LEN = 12;

enum StipType { ST_NONE = 0, ST_SOCIAL = 1, ST_ACADEMIC = 2, ST_INCREASED = 3 };

struct Student {
    char    zachetka[KEY_LEN];
    char    fio[FIO_LEN];
    char    group[GROUP_LEN];
    int32_t gpa100;
    uint8_t social;
    uint8_t stipType;
    int32_t amount;
    uint8_t deleted;
};

void clearStudent(Student& student);
void setText(char* dest, size_t size, const std::string& source);

bool validateKey(const std::string& text, std::string& errorText);
bool validateText(const std::string& text, size_t maxLen, const char* name, std::string& errorText);
bool parseGpa(const std::string& text, int32_t& result);

bool        isRecordValid(const Student& student);
void        calcStipend(Student& student);
const char* stipName(uint8_t type);
void        printHeader();
void        printStudent(const Student& student);
