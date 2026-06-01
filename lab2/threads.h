#pragma once
#include <string>
#include <windows.h>

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

    for (int i = 1; i < req->arr.size; i++) {
        if (req->arr.data[i] < min) {
            min = req->arr.data[i];
        }
        Sleep(7); 

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