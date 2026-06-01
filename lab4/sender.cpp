#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string>
#include "utilities.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    int thread_i;

    try {
        thread_i = std::stoi(argv[2]);
    }
    catch(std::exception e) {
        std::cout << e.what() << std::endl;
        return 1; 
    }

    char* mutexName = toCharStr("bebebe");
    HANDLE hMutex;
    hMutex = CreateMutex(NULL, FALSE, mutexName);
    delete[] mutexName;
    if (hMutex == NULL) {
        std::cout << "Fatal: couldn't create mutex\n";
        std::cout << "Error code: " << GetLastError() << std::endl;
        return 1;
    }
    
    std::ofstream fout(argv[1], std::ios::app | std::ios::binary);
    
    char* eventName = toCharStr("ready_" + std::to_string(thread_i));
    HANDLE hReadyEvent;
    hReadyEvent = CreateEvent(NULL, TRUE, FALSE, eventName);
    delete[] eventName;
    if (hReadyEvent == NULL) {
        std::cout << "Fatal: couldn't create mutex\n";
        std::cout << "Error code: " << GetLastError() << std::endl;
        return 1;
    }

    HANDLE hSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "sem");
    HANDLE hSemSender = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "sem-sender");
   

    SetEvent(hReadyEvent);

    bool isActive = 1;
    do {
        std::cout << "\n-----------\n";
        std::cout << "Sender Menu:\n";
        std::cout << "0: write message\n";
        std::cout << "1: exit\n";
        std::cout << "-----------\n\n";

        char choice = _getch();
        std::string message;
        char buffer[20] = { 0 };
        bool continueFlag = 0;

        switch (choice) {
        case '0':
            
            while (true) {
                std::cout << "Enter message: ";
                std::getline(std::cin, message);
                if (message.size() > 0 && message.size() <= 20) {
                    break;
                }
                std::cout << "\nError: invalid data. Message length must be more than 0 and less than 21 characters\n";
                std::cout << "Press '0' to cancel\n";
                std::cout << "Press any other key to try again\n\n";
                char f = _getch();
                if (f == '0') {
                    continueFlag = 1;
                    break;
                }
            }

            if (!continueFlag) {
                std::strncpy(buffer, message.data(), 20);
                std::cout << "Sending message...\n";
                WaitForSingleObject(hSemSender, INFINITE);
                WaitForSingleObject(hMutex, INFINITE);
                writefile(fout, buffer, sizeof(buffer));
                ReleaseMutex(hMutex);
                ReleaseSemaphore(hSem, 1, NULL);
                std::cout << "Message sent: " << message << std::endl;
            }
            break;
        case '1':
            isActive = 0;
            break;
        default:
            std::cout << "Choose valid option out of menu\n";
            break;
        }
    } while(isActive);
    fout.close();
    std::cout << "sender finishes its work";
    return 0;
}
