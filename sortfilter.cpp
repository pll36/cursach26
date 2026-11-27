#include "sortfilter.h"
#include "storage.h"
#include <cstdio>
#include <cstring>
#include <utility>

using namespace std;

vector<Student> loadActive(const string& path) {
    vector<Student> students;
    size_t count = countRecords(path);
    Student student;
    for (size_t i = 0; i < count; ++i) {
        readAt(path, i, student);
        if (!student.deleted) students.push_back(student);
    }
    return students;
}

static int compareStudents(const Student& a, const Student& b, int field) {
    if (field == SF_FIO) return strcmp(a.fio, b.fio);
    if (field == SF_GROUP) return strcmp(a.group, b.group);
    if (field == SF_GPA) return a.gpa100 - b.gpa100;
    return a.amount - b.amount;
}

void sortStudents(vector<Student>& students, int field, bool ascending) {
    for (size_t i = 0; i < students.size(); ++i) {
        for (size_t j = 0; j + 1 < students.size() - i; ++j) {
            int result = compareStudents(students[j], students[j + 1], field);
            if (!ascending) result = -result;
            if (result > 0) swap(students[j], students[j + 1]);
        }
    }
}

vector<Student> filterByGroup(const vector<Student>& students, const string& group) {
    vector<Student> result;
    for (size_t i = 0; i < students.size(); ++i)
        if (group == students[i].group) result.push_back(students[i]);
    return result;
}

vector<Student> filterByMinGpa(const vector<Student>& students, int32_t minGpa100) {
    vector<Student> result;
    for (size_t i = 0; i < students.size(); ++i)
        if (students[i].gpa100 >= minGpa100) result.push_back(students[i]);
    return result;
}

vector<Student> filterByType(const vector<Student>& students, uint8_t type) {
    vector<Student> result;
    for (size_t i = 0; i < students.size(); ++i)
        if (students[i].stipType == type) result.push_back(students[i]);
    return result;
}

void printList(const vector<Student>& students) {
    if (students.empty()) {
        printf("Нет записей.\n");
        return;
    }
    printHeader();
    for (size_t i = 0; i < students.size(); ++i) printStudent(students[i]);
}
