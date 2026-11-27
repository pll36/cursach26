#pragma once
#include <string>
#include <vector>
#include "student.h"

enum SortField { SF_FIO = 1, SF_GROUP = 2, SF_GPA = 3, SF_AMOUNT = 4 };

// Используется контейнер std::vector (ТЗ 4.3.4)
std::vector<Student> loadActive(const std::string& path);
void sortStudents(std::vector<Student>& students, int field, bool ascending);
std::vector<Student> filterByGroup(const std::vector<Student>& students, const std::string& group);
std::vector<Student> filterByMinGpa(const std::vector<Student>& students, int32_t minGpa100);
std::vector<Student> filterByType(const std::vector<Student>& students, uint8_t type);
void printList(const std::vector<Student>& students);
