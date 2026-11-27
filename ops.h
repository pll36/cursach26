#pragma once
#include <string>
#include "student.h"

// Без вспомогательного массива (ТЗ 4.2.4, 4.2.5)
bool   addRecord(const std::string& path, const Student& student);
int    findByKey(const std::string& path, const std::string& key);
void   viewAll(const std::string& path);
bool   markDeleted(const std::string& path, const std::string& key);
size_t compactFile(const std::string& path);
void   calcAll(const std::string& path);
