#include <iostream>
#include <string>
#include <windows.h>
#include "employee.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");

    if (argc < 3) {
        std::cerr << "Creator: неверное число аргументов\n";
        return 1;
    }

    std::string fileName = argv[1];
    int lines = 0;

    try {
        lines = std::stoi(argv[2]);
    }
    catch (std::exception e) {
        std::cerr << "Creator: неверное количество записей\n";
        return 1;
    }

    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Creator: " << GetWindowsErrorText(GetLastError()) << "\n";
        return GetLastError();
    }

    for (int i = 0; i < lines; i++) {
        employee emp;
        std::cout << "\nСотрудник [" << i + 1 << "/" << lines << "]\n";

        std::cout << "ID: ";
        try {
            std::cin >> emp.num;
            if (emp.num < 1) {
                throw std::exception("ID не может быть меньше 1");
            }
        }
        catch (std::exception& e) {
            std::cerr << e.what();
            return 1;
        }

        std::cout << "Имя: ";
        std::cin.ignore();
        std::cin.getline(emp.name, sizeof(emp.name));

        std::cout << "Часы: ";
        try {
            std::cin >> emp.hours;
            if (emp.hours < 0) {
                throw std::exception("Часы не могут быть меньше 0");
            }
        }
        catch (std::exception& e) {
            std::cerr << e.what();
            return 1;
        }

        DWORD bytesWritten;
        if (!WriteFile(hFile, &emp, sizeof(employee), &bytesWritten, NULL)) {
            std::cerr << "Ошибка записи в бинарный фалй: " << GetWindowsErrorText(GetLastError()) << "\n";
            CloseHandle(hFile);
            return 1;
        }
    }

    CloseHandle(hFile);
    return 0;
}