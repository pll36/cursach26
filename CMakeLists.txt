cmake_minimum_required(VERSION 3.16)
project(scholarship CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W3 /utf-8)      # исходники в UTF-8
else()
    add_compile_options(-Wall -Wextra)
endif()

# ---- ядро: записи, файлы, подпись, учётная запись (без интерфейса) ----
add_library(core STATIC
    student.cpp storage.cpp ops.cpp sortfilter.cpp crypto.cpp auth.cpp statement.cpp)

# ---- консольная версия (резервная, удобна для отладки) ----
add_executable(scholarship_cli main.cpp ui.cpp)
target_link_libraries(scholarship_cli core)

# ---- графическая версия (Qt Widgets, Qt 5 или Qt 6) ----
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets QUIET)
if(QT_FOUND)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
    set(CMAKE_AUTOMOC ON)
    add_executable(scholarship
        gui_main.cpp gui_util.cpp login_dialog.cpp student_dialog.cpp main_window.cpp)
    target_link_libraries(scholarship core Qt${QT_VERSION_MAJOR}::Widgets)
    if(WIN32)
        set_target_properties(scholarship PROPERTIES WIN32_EXECUTABLE TRUE)  # без окна консоли
        if(MSVC)
            target_link_options(scholarship PRIVATE /ENTRY:mainCRTStartup)
        endif()
    endif()
else()
    message(WARNING "Qt не найден: собирается только консольная версия scholarship_cli")
endif()
