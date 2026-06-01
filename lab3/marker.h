#pragma once

#include <windows.h>
#include <iostream>
#include <cstdlib>

struct ThreadInfo {
    int id;
    int arraySize;
    int* arr;
    CRITICAL_SECTION* cs;
    HANDLE hStartEvent;
    HANDLE hStopEvent;
    HANDLE hCantWorkEvent;
    HANDLE hMainContinueEvent;
    bool isTerminated;
};

inline DWORD WINAPI markerFunc(void* pArguments) {
    ThreadInfo* data = (ThreadInfo*)pArguments;
    srand(data->id);

    WaitForSingleObject(data->hStartEvent, INFINITE);

    int counter = 0;

    while (true) {
        int index = rand() % data->arraySize;

        EnterCriticalSection(data->cs);
        if (data->arr[index] == 0) {
            Sleep(5);
            data->arr[index] = data->id;
            counter++;
            Sleep(5);
            LeaveCriticalSection(data->cs);
        }
        else {
            std::cout << "\nMarker:" << data->id << std::endl;
            std::cout << "Marked: " << counter << std::endl;
            std::cout << "Blocked at index: " << index << std::endl;

            LeaveCriticalSection(data->cs);

            SetEvent(data->hCantWorkEvent);

            HANDLE waitEvents[2] = { data->hMainContinueEvent, data->hStopEvent };
            DWORD dwWait = WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);

            if (dwWait == WAIT_OBJECT_0 + 1) {
                EnterCriticalSection(data->cs);
                for (int i = 0; i < data->arraySize; i++) {
                    if (data->arr[i] == data->id) {
                        data->arr[i] = 0;
                    }
                }
                LeaveCriticalSection(data->cs);
                break;
            }
        }
    }

    return 0;
}
