#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include "common.h"
#include "rwmutex.h"

std::map<int, RWMutex*> record_locks;
std::map<int, int> record_offsets;
char filename[256];

void PrintFile() {
    HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось открыть файл\n";
        return;
    }

    employee emp;
    DWORD bytesRead;
    std::cout << "\n--- Содержимое файла ---\n";
    while (ReadFile(hFile, &emp, sizeof(employee), &bytesRead, NULL) && bytesRead == sizeof(employee)) {
        std::cout << "ID: " << emp.num << " | Имя: " << emp.name << " | Часы: " << emp.hours << "\n";
    }
    std::cout << "------------------------\n\n";
    CloseHandle(hFile);
}

DWORD WINAPI ClientHandler(LPVOID lpParam) {
    HANDLE hPipe = (HANDLE)lpParam;

    ConnectNamedPipe(hPipe, NULL);

    Request req;
    Response res;
    DWORD bytesProcessed;

    while (ReadFile(hPipe, &req, sizeof(Request), &bytesProcessed, NULL) && bytesProcessed == sizeof(Request)) {
        res.success = true;
        ZeroMemory(res.error_msg, sizeof(res.error_msg));

        if (req.cmd == CMD_QUIT) break;

        auto it = record_offsets.find(req.id);
        if (it == record_offsets.end()) {
            res.success = false;
            strcpy_s(res.error_msg, "Запись не найдена");
            WriteFile(hPipe, &res, sizeof(Response), &bytesProcessed, NULL);
            continue;
        }

        int offset = it->second;
        RWMutex* mtx = record_locks[req.id];
        HANDLE hFile;

        switch (req.cmd) {
        case CMD_GET_FOR_READ:
            mtx->RLock();
            hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
            ReadFile(hFile, &res.emp, sizeof(employee), &bytesProcessed, NULL);
            CloseHandle(hFile);
            break;
        case CMD_RELEASE_READ:
            mtx->RUnlock();
            break;
        case CMD_GET_FOR_MODIFY:
            mtx->Lock();
            hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
            ReadFile(hFile, &res.emp, sizeof(employee), &bytesProcessed, NULL);
            CloseHandle(hFile);
            break;
        case CMD_SAVE_MODIFY:
            hFile = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
            WriteFile(hFile, &req.emp, sizeof(employee), &bytesProcessed, NULL);
            CloseHandle(hFile);
            break;
        case CMD_RELEASE_MODIFY:
            mtx->Unlock();
            break;
        default:
            break;
        }

        WriteFile(hPipe, &res, sizeof(Response), &bytesProcessed, NULL);
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return 0;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::cout << "Введите имя файла: ";
    std::cin >> filename;

    int num_records;
    while (true) {
        std::cout << "Введите количество сотрудников: ";
        if (std::cin >> num_records && num_records > 0) {
            break;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Ошибка ввода! Введите натуральное число\n";
    }

    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    for (int i = 0; i < num_records; ++i) {
        employee emp;
        std::cout << "Сотрудник " << i + 1 << ":\nID: ";
        while (true) {
            std::cout << "ID (>0): ";
            if (std::cin >> emp.num && emp.num > 0) break;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Ошибка! Введите натуральное число\n";
        }

        while (true) {
            std::cout << "Имя (до 9 символов): ";
            if (std::cin >> emp.name) break;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }

        while (true) {
            std::cout << "Часы (>= 0): ";
            if (std::cin >> emp.hours && emp.hours >= 0) break;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Ошибка! Введите неотрицательное число\n";
        }

        DWORD bytesWritten;
        WriteFile(hFile, &emp, sizeof(employee), &bytesWritten, NULL);
    }
    CloseHandle(hFile);

    PrintFile();

    hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    employee emp;
    DWORD bytesRead;
    int current_offset = 0;
    while (ReadFile(hFile, &emp, sizeof(employee), &bytesRead, NULL) && bytesRead == sizeof(employee)) {
        record_offsets[emp.num] = current_offset;
        record_locks[emp.num] = new RWMutex();
        current_offset += sizeof(employee);
    }
    CloseHandle(hFile);

    int num_clients;
    while (true) {
        std::cout << "Введите количество процессов-клиентов (> 0): ";
        if (std::cin >> num_clients && num_clients > 0) break;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Ошибка ввода! Введите положительное целое число\n";
    }

    std::vector<HANDLE> threads;
    std::vector<HANDLE> client_processes;

    for (int i = 0; i < num_clients; ++i) {
        HANDLE hPipe = CreateNamedPipeA(
            PIPE_NAME, PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, sizeof(Response), sizeof(Request), 0, NULL
        );

        HANDLE hThread = CreateThread(NULL, 0, ClientHandler, (LPVOID)hPipe, 0, NULL);
        threads.push_back(hThread);
    }

    for (int i = 0; i < num_clients; ++i) {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFOA));
        si.cb = sizeof(STARTUPINFOA);

        if (CreateProcessA(NULL, "Client.exe", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            client_processes.push_back(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else {
            std::cerr << "Не удалось запустить Client.exe\n";
        }
    }

    WaitForMultipleObjects(client_processes.size(), client_processes.data(), TRUE, INFINITE);

    for (HANDLE hProc : client_processes) CloseHandle(hProc);
    for (HANDLE hThread : threads) CloseHandle(hThread);

    std::cout << "\nВсе клиенты завершили работу\n";
    PrintFile();

    for (auto& pair : record_locks) delete pair.second;

    std::cout << "Нажмите клавишу для завершения сервера";
    _getch();
    return 0;
}