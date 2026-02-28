#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <windows.h>
#include "employee.h"

void FormattedOutput(std::ofstream& fout, std::string s1, std::string s2, std::string s3, std::string s4) {
    fout << " | "  << std::left;
    fout << std::setw(20) << s1 << " | ";
    fout << std::setw(20) << s2 << " | ";
    fout << std::setw(20) << s3 << " | ";
    fout << std::right <<  std::setw(20) << s4 << " |\n";
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");

    if (argc < 4) {
        std::cerr << "Reporter: неверное число аргументов\n";
        return 1;
    }

    std::string binFileName = argv[1];
    std::string txtFileName = argv[2];
    double salary = 0;

    try {
        salary = std::stod(argv[3]);
    }
    catch (std::exception e) {
        std::cerr << "Reporter: невенрое значение зп/час\n";
        return 1;
    }

    HANDLE hFile = CreateFile(binFileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Reporter: " << GetWindowsErrorText(GetLastError()) << "\n";
        return GetLastError();
    }

    std::vector<employee> data;
    employee emp;
    DWORD dwBytesRead;

    while (ReadFile(hFile, &emp, sizeof(employee), &dwBytesRead, NULL)) {
        if (dwBytesRead == 0) {
            break;
        }
        data.push_back(emp);
    }
    CloseHandle(hFile);

    std::sort(data.begin(), data.end(), comparator);

    std::ofstream fout(txtFileName);
    if (!fout.is_open()) {
        std::cerr << "Reporter: не удалось создать текстовый файл\n";
        return 1;
    }

    fout << "Отчет по файлу \"" << binFileName << "\"\n";
    FormattedOutput(fout, "ID", "Имя", "Часы", "Зарплата");
    
    std::cout << data.size() << std::endl;
    for (int i = 0; i < data.size(); i++) {
        FormattedOutput(fout, std::to_string(data[i].num), data[i].name, std::to_string(data[i].hours), std::to_string(data[i].hours * salary));
    }

    fout.close();
    return 0;
}