#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include "employee.h"

void RunProcessAndWait(std::string commandLine) {
    STARTUPINFO si;      
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::vector<char> cmdBuffer(commandLine.begin(), commandLine.end());
    cmdBuffer.push_back('\0'); // нуль-терминатор

    if (!CreateProcess(NULL, cmdBuffer.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        throw std::runtime_error("Ошибка запуска процесса: " + GetWindowsErrorText(GetLastError()));
    }
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD ExitCode;
    GetExitCodeProcess(pi.hProcess, &ExitCode);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    if (ExitCode != 0) {
        throw std::exception("Ошибка во время выполнения процесса");
    }
}

void PrintBinaryFile(std::string fileName) {
    HANDLE hFile;
    
    hFile = CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == NULL) {
        throw "не удалось открыть файл";
    }

    std::cout << "\n--- Бинарный файл ---\n";
    employee emp;
    DWORD dwBytesRead;

    while (ReadFile(hFile, &emp, sizeof(emp), &dwBytesRead, NULL)) {
        if (dwBytesRead == 0) {
            break;
        }
        std::cout << emp.num << " " << emp.name << " " << emp.hours << "\n";
    }
    std::cout << "---------------------\n\n";

    CloseHandle(hFile);
}

void PrintTextFile(std::string fileName) {
    std::ifstream fin(fileName);
    if (!fin.is_open()) {
        throw std::exception("Ошибка: не удалось открыть файл отчета");
    }
    std::cout << "\n--- Файл отчета ---\n";
    std::string line;
    while (!fin.eof()) {
        std::getline(fin, line);
        std::cout << line << "\n";
    }
    std::cout << "-------------------\n\n";
}

bool isValid(std::string str) {
    for (int i = 0; i < size(str); i++) {
        if (!std::isalpha(str[i])) {
            return false;
        }
    }
    return true;
}

int main() {
    setlocale(LC_ALL, "RUS");

    try {
        std::string binName;
        int recordsCount;

        std::cout << "Имя бинарного файла: ";
        std::cin >> binName;
        if (isValid(binName)) {
            binName += ".dat";
        }
        else {
            throw std::exception("имя файла должно состоять только из латинских букв");
        }
        std::cout << "Количество записей: ";
        std::cin >> recordsCount;

        std::string cmdCreator = "Creator.exe " + binName + " " + std::to_string(recordsCount);
        std::cout << cmdCreator << std::endl;
        RunProcessAndWait(cmdCreator);

        PrintBinaryFile(binName);

        std::string txtName;
        double salary;

        std::cout << "Имя файла отчета: ";
        std::cin >> txtName;
        if (isValid(txtName)) {
            txtName += ".txt";
        }
        else {
            throw std::exception("имя файла должно состоять только из латинских букв");
        }
        std::cout << "Ставка в час: ";
        std::cin >> salary;

        std::string cmdReporter = "Reporter.exe " + binName + " " + txtName + " " + std::to_string(salary);
        
        RunProcessAndWait(cmdReporter);

        PrintTextFile(txtName);

    }
    catch (std::exception e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    std::cout << "Работа Main завершена.\n";
    return 0;
}