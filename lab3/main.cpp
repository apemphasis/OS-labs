#include <iostream>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include "marker.h"

int main() {
    int n;
    std::cout << "Enter array size: ";
    std::cin >> n;

    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        arr[i] = 0;
    }

    int markerCount;
    std::cout << "Enter number of markers: ";
    std::cin >> markerCount;

    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);

    HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    std::vector<ThreadInfo*> infos(markerCount);
    std::vector<HANDLE> hThreads(markerCount);
    std::vector<DWORD> IDThreads(markerCount);
    std::vector<HANDLE> hCantWorkEvents(markerCount);

    for (int i = 0; i < markerCount; ++i) {
        infos[i] = new ThreadInfo;
        infos[i]->id = i+1;
        infos[i]->arraySize = n;
        infos[i]->arr = arr;
        infos[i]->cs = &cs;
        infos[i]->hStartEvent = hStartEvent;
        infos[i]->hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        infos[i]->hCantWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        infos[i]->hMainContinueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        infos[i]->isTerminated = false;

        hCantWorkEvents[i] = infos[i]->hCantWorkEvent;
        hThreads[i] = CreateThread(NULL, 0, markerFunc, (void*)infos[i], 0, &IDThreads[i]);
    }

    SetEvent(hStartEvent);

    int activeMarkers = markerCount;
    while (activeMarkers > 0) {
        std::vector<HANDLE> currentWaitEvents;
        for (int i = 0; i < markerCount; ++i) {
            if (!infos[i]->isTerminated) {
                currentWaitEvents.push_back(infos[i]->hCantWorkEvent);
            }
        }
        WaitForMultipleObjects(currentWaitEvents.size(), currentWaitEvents.data(), TRUE, INFINITE);

        std::cout << "\nCurrent array state: ";
        for (int i = 0; i < n; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;

        int killId;
        bool found = false;
        while (!found) {
            std::cout << "Enter marker ID to stop: ";
            std::cin >> killId;
            if (killId > 0 && killId <= markerCount && !infos[killId - 1]->isTerminated) {
                found = true;
            }
            else {
                std::cout << "Invalid ID or marker already stopped." << std::endl;
            }
        }

        SetEvent(infos[killId - 1]->hStopEvent);
        WaitForSingleObject(hThreads[killId - 1], INFINITE);
        infos[killId - 1]->isTerminated = true;
        activeMarkers--;

        std::cout << "Array after marker #" << killId << " stopped: ";
        for (int i = 0; i < n; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;

        for (int i = 0; i < markerCount; ++i) {
            if (!infos[i]->isTerminated) {
                SetEvent(infos[i]->hMainContinueEvent);
            }
        }
    }

    
    for (int i = 0; i < markerCount; ++i) {
        CloseHandle(hThreads[i]);
        CloseHandle(infos[i]->hStopEvent);
        CloseHandle(infos[i]->hCantWorkEvent);
        CloseHandle(infos[i]->hMainContinueEvent);
        delete infos[i];
    }
    CloseHandle(hStartEvent);
    DeleteCriticalSection(&cs);
    delete[] arr;

    std::cout << "\nAll markers finished" << std::endl;
    return 0;
}