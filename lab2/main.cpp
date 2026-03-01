#include <iostream>
#include <windows.h>
#include <string>

struct Array {
    int* data;
    int size;
};

struct MinMax {
    Array arr;
    int min;
    int max;
};

struct Average {
    Array arr;
    double avg;
};

DWORD WINAPI min_max(LPVOID lpParam) {
    MinMax* req = static_cast<MinMax*>(lpParam);
    int min = req->arr.data[0], max = req->arr.data[0];

    for (int i = 1; i < req->arr.size; i++){
        if (req->arr.data[i] < min) {
            min = req->arr.data[i];
        }

        if (req->arr.data[i] > max) {
            max = req->arr.data[i];
        }
        Sleep(7);
    }

    req->min = min;
    req->max = max;

    std::string out_str = "Min: " + std::to_string(req->min) + "\nMax: " + std::to_string(req->max) + "\n";

    std::cout << out_str; 
    return 0;
}

DWORD WINAPI average(LPVOID lpParam) {
    Average* req = static_cast<Average*>(lpParam);
    int sum = req->arr.data[0];

    for (int i = 1; i < req->arr.size; i++) {
        sum += req->arr.data[i];
        Sleep(12);
    }

    req->avg = static_cast<double>(sum) / static_cast<double>(req->arr.size);

    std::string out_str = "Average: " + std::to_string(req->avg) + "\n";

    std::cout << out_str;
    return 0;
}

int main() {

    Array arr;

    std::cout << "Enter size of array: ";
    if (!(std::cin >> arr.size)) {
        std::cerr << "not a number";
        return 1;
    }

    if (arr.size <= 0) {
        std::cerr << "size must be at least 1";
        return 1;
    }

    arr.data = new int[arr.size];

    for(int i = 0; i < arr.size; i++){ 
        std::cout << "Enter element [" << i+1 << "/" << arr.size << "]: ";
        if (!(std::cin >> arr.data[i])) {
            std::cerr << "not a number";
            delete[] arr.data;
            return 1;
        }
    }

    MinMax min_max_request = { arr, 0, 0 };

    Average avg_request = { arr, 0.0 };

    HANDLE min_max_hThread, average_hThread;
    DWORD min_max_IDThread, average_IDThread;

    min_max_hThread = CreateThread(NULL, 0, min_max, static_cast<LPVOID>(&min_max_request), 0, &min_max_IDThread);
    if (min_max_hThread == NULL) {
        std::cout << "Failed to start min/max thread";
        delete[] arr.data;
        return 1;
    }

    average_hThread = CreateThread(NULL, 0, average, static_cast<LPVOID>(&avg_request), 0, &average_IDThread);
    if (average_hThread == NULL) {
        std::cout << "Failed to start average thread";
        WaitForSingleObject(min_max_hThread, INFINITE);
        CloseHandle(min_max_hThread);
        delete[] arr.data;
        return 1;
    }

    WaitForSingleObject(min_max_hThread, INFINITE);
    CloseHandle(min_max_hThread);

    WaitForSingleObject(average_hThread, INFINITE);
    CloseHandle(average_hThread);

    for (int i = 0; i < arr.size; i++) {
        if (arr.data[i] == min_max_request.max || arr.data[i] == min_max_request.min) {
            arr.data[i] = static_cast<int>(avg_request.avg);
        }
        std::cout << arr.data[i] << " ";
    }

    delete[] arr.data;

    std::cout << "\nFinished successfuly";
    return 0;
}