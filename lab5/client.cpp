#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include "common.h"

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    HANDLE hPipe;
    while (true) {
        hPipe = CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE) break;
        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "Не удалось подключиться к каналу.\n";
            system("pause");
            return 1;
        }
        WaitNamedPipeA(PIPE_NAME, 5000);
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(hPipe, &mode, NULL, NULL);

    Request req;
    Response res;
    DWORD bytesProcessed;

    while (true) {
        std::cout << "\n1. Чтение записи\n2. Модификация записи\n3. Выход\nВыберите действие: ";
        int choice;
        while (true) {
            std::cout << "\n1. Чтение записи\n2. Модификация записи\n3. Выход\nВыберите действие (1-3): ";
            if (std::cin >> choice && choice >= 1 && choice <= 3) break;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Ошибка! Введите цифру 1, 2 или 3\n";
        }

        if (choice == 1) {
            while (true) {
                std::cout << "Введите ID сотрудника: ";
                if (std::cin >> req.id) break;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Ошибка! Ожидается число\n";
            }
            req.cmd = CMD_GET_FOR_READ;

            TransactNamedPipe(hPipe, &req, sizeof(Request), &res, sizeof(Response), &bytesProcessed, NULL);

            if (!res.success) {
                std::cout << "Ошибка: " << res.error_msg << "\n";
                continue;
            }

            std::cout << "Данные: " << res.emp.name << " | " << res.emp.hours << " ч\n";
            std::cout << "Нажмите клавишу для завершения: ";
            _getch();

            req.cmd = CMD_RELEASE_READ;
            TransactNamedPipe(hPipe, &req, sizeof(Request), &res, sizeof(Response), &bytesProcessed, NULL);
            std::cout << "Доступ освобожден\n";

        }
        else if (choice == 2) {
            while (true) {
                std::cout << "Введите ID сотрудника: ";
                if (std::cin >> req.id) break;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Ошибка! Ожидается число\n";
            }
            req.cmd = CMD_GET_FOR_MODIFY;

            TransactNamedPipe(hPipe, &req, sizeof(Request), &res, sizeof(Response), &bytesProcessed, NULL);

            if (!res.success) {
                std::cout << "Ошибка: " << res.error_msg << "\n";
                continue;
            }

            std::cout << "Текущие данные: " << res.emp.name << " | " << res.emp.hours << " ч\n";
            req.emp.num = req.id;
            while (true) {
                std::cout << "Новое имя (до 9 символов): ";
                if (std::cin >> req.emp.name) break;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }

            while (true) {
                std::cout << "Новые часы (>= 0): ";
                if (std::cin >> req.emp.hours && req.emp.hours >= 0) break;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Ошибка! Введите неотрицательное число\n";
            }

            std::cout << "Нажмите клавишу для сохранения данных на сервере: ";
            _getch();

            req.cmd = CMD_SAVE_MODIFY;
            TransactNamedPipe(hPipe, &req, sizeof(Request), &res, sizeof(Response), &bytesProcessed, NULL);
            std::cout << "Данные сохранены\n";

            std::cout << "Нажмите клавишу для завершения: ";
            _getch();

            req.cmd = CMD_RELEASE_MODIFY;
            TransactNamedPipe(hPipe, &req, sizeof(Request), &res, sizeof(Response), &bytesProcessed, NULL);
            std::cout << "Доступ освобожден\n";

        }
        else if (choice == 3) { // ВЫХОД
            req.cmd = CMD_QUIT;
            TransactNamedPipe(hPipe, &req, sizeof(Request), &res, sizeof(Response), &bytesProcessed, NULL);
            break;
        }
    }

    CloseHandle(hPipe);
    return 0;
}