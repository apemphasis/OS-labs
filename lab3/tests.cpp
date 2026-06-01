#include <iostream>
#include <cassert>
#include <windows.h>
#include "marker.h"

void TestFunc(int arg, int n, int* temp) {
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        arr[i] = 0;
    }

    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    ThreadInfo info = {
        arg,
        n,
        arr,
        &cs,
        hStartEvent,
        CreateEvent(NULL, FALSE, FALSE, NULL),
        CreateEvent(NULL, FALSE, FALSE, NULL),
        CreateEvent(NULL, FALSE, FALSE, NULL),
        false
    };
    DWORD IDThread;
    HANDLE hThread = CreateThread(NULL, 0, markerFunc, (void*)&info, 0, &IDThread);

    SetEvent(hStartEvent);

    WaitForSingleObject(info.hCantWorkEvent, INFINITE);

    for (int i = 0; i < 10; i++) {
        assert(arr[i] == temp[i] && "Arrays must be equal");
    }
    std::cout << "[OK] Test Passed!\n";

    SetEvent(info.hStopEvent);
    WaitForSingleObject(hThread, INFINITE);


    CloseHandle(hThread);
    CloseHandle(info.hStopEvent);
    CloseHandle(info.hCantWorkEvent);
    CloseHandle(info.hMainContinueEvent);
    CloseHandle(hStartEvent);
    DeleteCriticalSection(&cs);
    delete[] arr;
}



int main() {
    std::cout << "--- Running Unit Tests ---\n";
    int temp1[10] = { 1, 1, 0, 0, 1, 0, 0, 1, 0, 1 };
    TestFunc(1, 10, temp1);
    int temp2[10] = { 0, 0, 0, 0, 0, 2, 2, 0, 2, 0 };
    TestFunc(2, 10, temp2);
    std::cout << "--- All tests passed! ---\n";
    return 0;
}