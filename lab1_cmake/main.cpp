#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <limits>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

bool fileExists(const string& filename) {
    const ifstream f(filename.c_str());
    return f.good();
}

void printBinaryFile(const string& filename) {
    ifstream in(filename, ios::binary);
    if (!in) {
        cerr << "Cannot open binary file for reading." << endl;
        return;
    }

    cout << "\nBinary file contents:\n";
    cout << "----------------------------------------\n";
    cout << left << setw(10) << "ID" << setw(15) << "Name" << "Hours\n";
    cout << "----------------------------------------\n";

    employee emp{};
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        cout << left << setw(10) << emp.num
             << setw(15) << emp.name
             << fixed << setprecision(2) << emp.hours << "\n";
    }
    cout << "----------------------------------------\n";
    in.close();
}

void printReportFile(const string& filename) {
    ifstream report(filename);
    if (!report) {
        cerr << "Cannot open report file for reading." << endl;
        return;
    }

    cout << "\nReport contents:\n";
    string line;
    while (getline(report, line)) {
        cout << line << endl;
    }
    report.close();
}

int main() {
    string binFileName;
    int recordCount;

    cout << "========================================\n";
    cout << "Process Creation Laboratory Work\n";
    cout << "========================================\n\n";

    cout << "Enter binary file name: ";
    cin >> binFileName;

    cout << "Enter number of records: ";
    while (!(cin >> recordCount) || recordCount <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a positive integer: ";
    }

    if (fileExists(binFileName)) {
        remove(binFileName.c_str());
    }

    cout << "\nStarting Creator...\n";
    string cmdLine = "Creator.exe \"" + binFileName + "\" " + to_string(recordCount);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION piCreator;

    char* cmdLineStr = new char[cmdLine.length() + 1];
    strcpy(cmdLineStr, cmdLine.c_str());

    if (!CreateProcess(nullptr, cmdLineStr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &piCreator)) {
        cerr << "Failed to start Creator. Error code: " << GetLastError() << endl;
        delete[] cmdLineStr;
        return 1;
    }
    delete[] cmdLineStr;

    WaitForSingleObject(piCreator.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(piCreator.hProcess, &exitCode);

    CloseHandle(piCreator.hProcess);
    CloseHandle(piCreator.hThread);

    if (exitCode != 0) {
        cerr << "Creator failed with exit code: " << exitCode << endl;
        return 1;
    }

    if (!fileExists(binFileName)) {
        cerr << "Binary file was not created." << endl;
        return 1;
    }

    printBinaryFile(binFileName);

    string reportFileName;
    double rate;

    cout << "\nEnter report file name: ";
    cin >> reportFileName;

    cout << "Enter hourly rate: ";
    while (!(cin >> rate) || rate <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a positive number: ";
    }

    if (fileExists(reportFileName)) {
        remove(reportFileName.c_str());
    }

    cout << "\nStarting Reporter...\n";
    cmdLine = "Reporter.exe \"" + binFileName + "\" \"" + reportFileName + "\" " + to_string(rate);

    cmdLineStr = new char[cmdLine.length() + 1];
    strcpy(cmdLineStr, cmdLine.c_str());

    PROCESS_INFORMATION piReporter;
    if (!CreateProcess(nullptr, cmdLineStr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &piReporter)) {
        cerr << "Failed to start Reporter. Error code: " << GetLastError() << endl;
        delete[] cmdLineStr;
        return 1;
    }
    delete[] cmdLineStr;

    WaitForSingleObject(piReporter.hProcess, INFINITE);

    GetExitCodeProcess(piReporter.hProcess, &exitCode);

    CloseHandle(piReporter.hProcess);
    CloseHandle(piReporter.hThread);

    if (exitCode != 0) {
        cerr << "Reporter failed with exit code: " << exitCode << endl;
        return 1;
    }

    if (!fileExists(reportFileName)) {
        cerr << "Report file was not created." << endl;
        return 1;
    }

    printReportFile(reportFileName);

    cout << "\nProgram completed successfully.\n";
    return 0;
}