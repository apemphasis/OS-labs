#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <string>
#include <conio.h>
#include "utilities.h"

int main() {

    std::string filename;
    int numberOfReports;

    std::cout << "Enter file name: ";
    std::cin >> filename;
    
    std::ifstream fin;
    fin.open(filename, std::ios::binary);
    if (!fin.is_open()) {
        std::cout << "\nWarning: file didn't exist. ";
        std::ofstream fout(filename);
        if (!fout.is_open()) {
            std::cout << "Fatal: file with this name can't be created";
            return 1;
        }
        std::cout << "New file was created: " << filename << std::endl << std::endl;
        fout.close();
        fin.open(filename, std::ios::binary);
    }

    std::cout << "Enter numer of reports: ";
   
    while (true) {
        if ((std::cin >> numberOfReports) && numberOfReports >= 0) {
            break;
        }
        else {
            std::cin.clear();                 
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number no less than 0: ";
        }
    }

    /*for (int i = 0; i < numberOfReports; i++) {
        char s[20];
        int bytes = readfile(fin, s, sizeof(s));
        if (bytes != 20) {
            break;
        }
    }*/

    char* mutexName = toCharStr("bebebe");
    HANDLE hMutex;
    hMutex = CreateMutex(NULL, FALSE, mutexName);
    delete[] mutexName;

    HANDLE hSem = CreateSemaphore(NULL, 0, numberOfReports, "sem");
    HANDLE hSemSender = CreateSemaphore(NULL, numberOfReports, numberOfReports, "sem-sender");

    int sendersNum;
    std::cout << "Enter number of senders from interval [1; 20] : ";
    while (true) {
        if ((std::cin >> sendersNum) && sendersNum >= 1 && sendersNum <= 20) {
            break;
        }
        else {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number from interval [1; 20] : ";
        }
    }

    std::vector<HANDLE> hProcesses(sendersNum);
    std::vector<HANDLE> hThreads(sendersNum);
    std::vector<HANDLE> hReadyEvents(sendersNum);

    for (int i = 0; i < sendersNum; i++) {
        char* eventName = toCharStr("ready_" + std::to_string(i));
        hReadyEvents[i] = CreateEvent(NULL, TRUE, FALSE, eventName);
        delete[] eventName;
        if (hReadyEvents[i] == NULL) {
            std::cout << "Fatal: couldn't create ready_event\n";
            std::cout << "Error code: " << GetLastError() << std::endl;
            return 1;
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi; 

        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);

        char* cmd = toCharStr("sender " + filename + " " + std::to_string(i));
        BOOL flag = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
        delete[] cmd;
        if (!flag) {
            std::cout << "Fatal: couldn't create process\n";
            std::cout << "Error code: " << GetLastError() << std::endl;
            return 1;
        }

        hProcesses[i] = pi.hProcess;
        hThreads[i] = pi.hThread;
    }

    WaitForMultipleObjects(sendersNum, hReadyEvents.data(), TRUE, INFINITE);
    std::cout << "\nNotify: Senders are ready\n";

    int messageCount = 0;
    bool isActive = 1;
    do {

        std::cout << "\n-----------\n";
        std::cout << "Reciever Menu:\n";
        std::cout << "0: read message\n";
        std::cout << "1: exit\n";
        std::cout << "-----------\n\n";
        char choice = _getch();

        char buffer[21] = { 0 };
        int bytes;
        switch (choice) {
        case '0':
            std::cout << "Wating for message...\n";
            WaitForSingleObject(hSem, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);
            bytes = readfile(fin, buffer, sizeof(buffer));
            ReleaseMutex(hMutex);
            ReleaseSemaphore(hSemSender, 1, NULL);
            if (bytes == 20) {
                buffer[20] = '\0';
                std::cout << "Message: " << buffer << std::endl;
                messageCount++;
            }
            else {
                std::cout << "No new messages\n";
            }
            break;
        case '1':
            isActive = 0;
            break;
        default:
            std::cout << "Choose valid option out of menu\n";
            break;
        }
    } while (isActive);


    std::cout << "Waiting for all senders to finish...\n";
    
    WaitForMultipleObjects(sendersNum, hThreads.data(), TRUE, INFINITE);
    for (int i = 0; i < sendersNum; i++) {
        CloseHandle(hThreads[i]);
    }
    WaitForMultipleObjects(sendersNum, hProcesses.data(), TRUE, INFINITE);
    for (int i = 0; i < sendersNum; i++) {
        CloseHandle(hProcesses[i]);
    }

    std::cout << "All senders finished\n";
    std::cout << "Reciever read " << messageCount << " messages\n";

    fin.close();
    return 0;
}