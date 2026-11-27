#pragma once
#include <string>
#include "student.h"

bool   fileExists(const std::string& path);
void   createEmpty(const std::string& path);
size_t countRecords(const std::string& path);
void   readAt(const std::string& path, size_t index, Student& student);
void   writeAt(const std::string& path, size_t index, const Student& student);
void   appendRecord(const std::string& path, const Student& student);
void   replaceFile(const std::string& tmpPath, const std::string& target);
