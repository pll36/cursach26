#include <cstdio>
#include <exception>
#include "common.h"
#include "ui.h"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    try {
        showSplash();
        if (!doLogin()) {
            printf("Доступ запрещён.\n");
            return 1;
        }
        runMenu();
    } catch (const FileError& error) {
        printf("Критическая ошибка файла: %s\n", error.what());
        return 2;
    } catch (const exception& error) {
        printf("Критическая ошибка: %s\n", error.what());
        return 3;
    }
    return 0;
}
