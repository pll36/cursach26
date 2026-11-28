#include "ui.h"
#include "auth.h"
#include "common.h"
#include "crypto.h"
#include "ops.h"
#include "sortfilter.h"
#include "statement.h"
#include "storage.h"
#include "student.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct OperationCancelled {};

static string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        cout << "\nВвод завершён. Выход.\n";
        exit(0);
    }
    size_t begin = line.find_first_not_of(" \t\r");
    size_t end = line.find_last_not_of(" \t\r");
    if (begin == string::npos) return "";
    return line.substr(begin, end - begin + 1);
}

static int readInt(const string& prompt, int minValue, int maxValue) {
    for (;;) {
        string text = readLine(prompt);
        bool isNumber = !text.empty() && text.size() <= 6;
        for (size_t i = 0; isNumber && i < text.size(); ++i)
            if (text[i] < '0' || text[i] > '9') isNumber = false;
        if (isNumber) {
            int value = atoi(text.c_str());
            if (value >= minValue && value <= maxValue) return value;
        }
        printf("Ошибка: введите целое число от %d до %d.\n", minValue, maxValue);
    }
}

static bool confirm(const string& question) {
    for (;;) {
        string answer = readLine(question + " (д/н): ");
        if (answer == "д" || answer == "Д" || answer == "y" || answer == "Y") return true;
        if (answer == "н" || answer == "Н" || answer == "n" || answer == "N") return false;
        printf("Введите «д» или «н».\n");
    }
}

// Сообщает о неверном поле и предлагает повторить ввод или отменить операцию (ТЗ 4.2.2)
static void failOrCancel(const string& errorText) {
    printf("Ошибка: %s\n", errorText.c_str());
    if (!confirm("Повторить ввод?")) throw OperationCancelled();
}

static string askKey() {
    for (;;) {
        string errorText;
        string text = readLine("Номер зачётки (до 11 латинских букв и цифр): ");
        if (validateKey(text, errorText)) return text;
        failOrCancel(errorText);
    }
}

static string askText(const char* prompt, size_t maxLen, const char* name, bool allowCancel = true) {
    for (;;) {
        string errorText;
        string text = readLine(prompt);
        if (validateText(text, maxLen, name, errorText)) return text;
        if (allowCancel) failOrCancel(errorText);
        else printf("Ошибка: %s\n", errorText.c_str());
    }
}

static int32_t askGpa(const char* prompt) {
    for (;;) {
        int32_t gpa;
        if (parseGpa(readLine(prompt), gpa)) return gpa;
        failOrCancel("средний балл должен быть от 2.00 до 5.00 (не более 2 знаков после точки)");
    }
}

static string askTextOptional(const char* prompt, size_t maxLen, const char* name) {
    for (;;) {
        string errorText;
        string text = readLine(prompt);
        if (text.empty() || validateText(text, maxLen, name, errorText)) return text;
        failOrCancel(errorText);
    }
}

static bool askGpaOptional(const char* prompt, int32_t& gpa) {
    for (;;) {
        string text = readLine(prompt);
        if (text.empty()) return false;
        if (parseGpa(text, gpa)) return true;
        failOrCancel("средний балл должен быть от 2.00 до 5.00 (не более 2 знаков после точки)");
    }
}

static void offerView(const char* path) {
    if (confirm("Показать содержимое файла?")) viewAll(path);
}

void showSplash() {
    printf("==============================================\n");
    printf("  ИС начисления стипендий (ИСНС-ЭП)\n");
    printf("  с имитацией ЭП ведомостей. Курсовая работа\n");
    printf("==============================================\n");
}

bool doLogin() {
    if (!usersExist()) {
        printf("Первый запуск: создайте учётную запись.\n");
        string login = askText("Логин (до 31 байта): ", 32, "логин", false);
        for (;;) {
            string password = readLine("Пароль (не короче 4 символов): ");
            if (password.size() < 4) {
                printf("Ошибка: пароль слишком короткий.\n");
                continue;
            }
            if (password != readLine("Повторите пароль: ")) {
                printf("Пароли не совпадают.\n");
                continue;
            }
            createUser(login, password);
            break;
        }
        printf("Учётная запись создана.\n");
    }
    for (int attempt = 1; attempt <= 3; ++attempt) {
        string login = readLine("Логин: ");
        string password = readLine("Пароль: ");
        try {
            if (checkLogin(login, password)) return true;
        } catch (const FileError& error) {
            printf("Ошибка: %s.\nУдалите файл %s и запустите программу снова "
                        "для повторной настройки учётной записи.\n", error.what(), FILE_USERS);
            return false;
        }
        printf("Неверный логин или пароль (попытка %d из 3).\n", attempt);
    }
    return false;
}

static void actCreate() {
    if (fileExists(FILE_STUDENTS) &&
        !confirm("Файл уже существует, все данные будут потеряны. Продолжить?")) {
        printf("Операция отменена. Данные не изменены.\n");
        return;
    }
    createEmpty(FILE_STUDENTS);
    printf("Пустой файл создан.\n");
}

static void actAdd() {
    string key;
    for (;;) {
        key = askKey();
        if (!fileExists(FILE_STUDENTS) || findByKey(FILE_STUDENTS, key) < 0) break;
        failOrCancel("запись с таким номером зачётки уже существует");
    }
    Student student;
    clearStudent(student);
    setText(student.zachetka, KEY_LEN, key);
    setText(student.fio, FIO_LEN, askText("ФИО (до 95 байт, кириллица - 2 байта на букву): ", FIO_LEN, "ФИО"));
    setText(student.group, GROUP_LEN, askText("Группа (до 11 байт, например АБ-11): ", GROUP_LEN, "группа"));
    student.gpa100 = askGpa("Средний балл от 2.00 до 5.00 (например 4.57): ");
    student.social = confirm("Есть право на социальную стипендию?") ? 1 : 0;
    calcStipend(student);

    bool fileExistedBefore = fileExists(FILE_STUDENTS);
    if (!addRecord(FILE_STUDENTS, student)) {
        printf("Ошибка: запись не добавлена (ключ уже существует).\n");
        return;
    }
    // Сообщение о создании файла выводится после успешной записи (ТЗ 4.2.2)
    if (!fileExistedBefore) printf("Файл %s создан.\n", FILE_STUDENTS);
    printf("Запись добавлена.\n");
    offerView(FILE_STUDENTS);
}

static void actEdit() {
    string key = askKey();
    int index = findByKey(FILE_STUDENTS, key);
    if (index < 0) {
        printf("Запись не найдена.\n");
        return;
    }
    Student student;
    readAt(FILE_STUDENTS, index, student);
    printHeader();
    printStudent(student);
    printf("Для каждого поля: Enter - оставить значение без изменений.\n");

    string text = askTextOptional("Новое ФИО: ", FIO_LEN, "ФИО");
    if (!text.empty()) setText(student.fio, FIO_LEN, text);
    text = askTextOptional("Новая группа: ", GROUP_LEN, "группа");
    if (!text.empty()) setText(student.group, GROUP_LEN, text);
    int32_t gpa;
    if (askGpaOptional("Новый средний балл (2.00-5.00): ", gpa)) student.gpa100 = gpa;

    if (!confirm("Сохранить изменения?")) {
        printf("Операция отменена. Данные не изменены.\n");
        return;
    }
    calcStipend(student);
    writeAt(FILE_STUDENTS, index, student);
    printf("Запись изменена.\n");
    offerView(FILE_STUDENTS);
}

static void actDelete() {
    string key = askKey();
    int index = findByKey(FILE_STUDENTS, key);
    if (index < 0) {
        printf("Запись не найдена.\n");
        return;
    }
    Student student;
    readAt(FILE_STUDENTS, index, student);
    printHeader();
    printStudent(student);
    if (!confirm("Удалить эту запись?")) {
        printf("Операция отменена. Данные не изменены.\n");
        return;
    }
    markDeleted(FILE_STUDENTS, key);
    printf("Запись удалена (помечена как удалённая).\n");
    offerView(FILE_STUDENTS);
}

static void actFind() {
    string key = askKey();
    // Поиск без вспомогательного массива (ТЗ 4.2.5)
    int index = findByKey(FILE_STUDENTS, key);
    if (index < 0) {
        printf("Совпадений не найдено.\n");
        return;
    }
    Student student;
    readAt(FILE_STUDENTS, index, student);
    printHeader();
    printStudent(student);
}

static void actSort() {
    vector<Student> students = loadActive(FILE_STUDENTS);
    printf("Поле: 1-ФИО, 2-группа, 3-средний балл, 4-сумма\n");
    int field = readInt("Выбор (1-4): ", 1, 4);
    int direction = readInt("Направление: 1-по возрастанию, 2-по убыванию: ", 1, 2);
    sortStudents(students, field, direction == 1);
    printList(students);
}

static void actFilter() {
    vector<Student> students = loadActive(FILE_STUDENTS);
    printf("Условие: 1-группа, 2-средний балл не ниже, 3-вид стипендии\n");
    int mode = readInt("Выбор (1-3): ", 1, 3);
    if (mode == 1) {
        printList(filterByGroup(students, readLine("Группа (точное совпадение): ")));
    } else if (mode == 2) {
        printList(filterByMinGpa(students, askGpa("Минимальный балл (2.00-5.00): ")));
    } else {
        printf("Вид: 0-нет, 1-социальная, 2-академическая, 3-повышенная\n");
        printList(filterByType(students, readInt("Выбор (0-3): ", 0, 3)));
    }
}

static void printSign(const SignInfo& info) {
    printf("Хеш файла h        = %u\n", info.hash);
    printf("h mod n (n = %u) = %u\n", RSA_N, info.hashMod);
    printf("Подпись s          = %u\n", info.sig);
}

static void actForm() {
    string group = readLine("Группа для ведомости (Enter - все группы): ");
    size_t count = formStatement(FILE_STUDENTS, FILE_STATEMENT, group);
    SignInfo info;
    signStatement(FILE_STATEMENT, FILE_SIGNATURE, info);
    cout << "Ведомость сформирована и подписана (записей: " << count << ").\n";
    if (count == 0) printf("Внимание: подходящих записей нет, ведомость пуста.\n");
    printSign(info);
}

static void actVerify() {
    SignInfo info;
    verifyStatement(FILE_STATEMENT, FILE_SIGNATURE, info);
    printSign(info);
    if (info.valid) printf("РЕЗУЛЬТАТ: подпись ВЕРНА, ведомость не изменялась.\n");
    else printf("РЕЗУЛЬТАТ: подпись НЕДЕЙСТВИТЕЛЬНА, ведомость изменена!\n");
}

static void actCompact() {
    size_t removed = compactFile(FILE_STUDENTS);
    cout << "Файл упакован. Физически удалено записей: " << removed << "\n";
}

static void actTamper() {
    if (!confirm("Испортить ведомость для проверки подписи?")) {
        printf("Операция отменена. Данные не изменены.\n");
        return;
    }
    tamperStatement(FILE_STATEMENT);
    printf("Один байт ведомости изменён. Выполните проверку подписи.\n");
}

void runMenu() {
    for (;;) {
        printf("\n--- МЕНЮ ---\n");
        printf(" 1. Создать пустой файл данных\n");
        printf(" 2. Просмотр файла\n");
        printf(" 3. Добавить запись\n");
        printf(" 4. Изменить запись\n");
        printf(" 5. Удалить запись\n");
        printf(" 6. Поиск по номеру зачётки\n");
        printf(" 7. Пересчитать стипендии\n");
        printf(" 8. Сортировка\n");
        printf(" 9. Фильтрация\n");
        printf("10. Сформировать и подписать ведомость\n");
        printf("11. Просмотр ведомости\n");
        printf("12. Проверить подпись ведомости\n");
        printf("13. Упаковать файл (убрать удалённые)\n");
        printf("14. Испортить ведомость (демонстрация)\n");
        printf(" 0. Выход\n");
        int choice = readInt("Выбор (0-14): ", 0, 14);
        if (choice == 0) {
            printf("Работа завершена. Все файлы закрыты.\n");
            return;
        }
        try {
            switch (choice) {
                case 1:  actCreate(); break;
                case 2:  viewAll(FILE_STUDENTS); break;
                case 3:  actAdd(); break;
                case 4:  actEdit(); break;
                case 5:  actDelete(); break;
                case 6:  actFind(); break;
                case 7:  calcAll(FILE_STUDENTS); printf("Стипендии пересчитаны.\n"); break;
                case 8:  actSort(); break;
                case 9:  actFilter(); break;
                case 10: actForm(); break;
                case 11: viewAll(FILE_STATEMENT); break;
                case 12: actVerify(); break;
                case 13: actCompact(); break;
                case 14: actTamper(); break;
            }
        } catch (const OperationCancelled&) {
            printf("Операция отменена. Данные не изменены.\n");
        } catch (const FileError& error) {
            printf("Ошибка файла: %s. Операция не выполнена.\n", error.what());
        } catch (const exception& error) {
            printf("Ошибка: %s. Операция не выполнена.\n", error.what());
        }
    }
}
