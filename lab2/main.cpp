#include <iostream>
#include <windows.h>
#include <string>

int size;
int* arr;

int min = 0, max = 0;
double avg;


DWORD WINAPI min_max(LPVOID lpParam) {
    min = arr[0], max = arr[0];

    for (int i = 1; i < size; i++){
        if (arr[i] < min) {
            min = arr[i];
        }

        if (arr[i] > max) {
            max = arr[i];
        }
        Sleep(7);
    }

    std::string out_str = "Min: " + std::to_string(min) + "\nMax: " + std::to_string(max) + "\n";

    std::cout << out_str; 
    return 0;
}

DWORD WINAPI average(LPVOID lpParam) {
    int sum = arr[0];

    for (int i = 1; i < size; i++) {
        sum += arr[i];
        Sleep(12);
    }

    avg = static_cast<double>(sum) / static_cast<double>(size);

    std::string out_str = "Average: " + std::to_string(avg) + "\n";

    std::cout << out_str;
    return 0;
}

int main() {

    std::cout << "Enter size of array: ";
    if (!(std::cin >> size)) {
        std::cerr << "not a number";
        return 1;
    }

    if (size <= 0) {
        std::cerr << "size must be at least 1";
        return 1;
    }

    arr = new int[size];

    for(int i = 0; i < size; i++){ 
        std::cout << "Enter element [" << i+1 << "/" << size << "]: ";
        if (!(std::cin >> arr[i])) {
            std::cerr << "not a number";
            delete[] arr;
            return 1;
        }
    }

    HANDLE min_max_hThread;
    DWORD min_max_IDThread;

    min_max_hThread = CreateThread(NULL, 0, min_max, NULL, 0, &min_max_IDThread);
    if (min_max_hThread == NULL) {
        std::cout << "Failed to start min/max thread";
        delete[] arr;
        return 1;
    }

    HANDLE average_hThread;
    DWORD average_IDThread;

    average_hThread = CreateThread(NULL, 0, average, NULL, 0, &average_IDThread);
    if (average_hThread == NULL) {
        std::cout << "Failed to start average thread";
        CloseHandle(min_max_hThread);
        delete[] arr;
        return 1;
    }

    WaitForSingleObject(min_max_hThread, INFINITE);
    CloseHandle(min_max_hThread);

    WaitForSingleObject(average_hThread, INFINITE);
    CloseHandle(average_hThread);

    for (int i = 0; i < size; i++) {
        if (arr[i] == max || arr[i] == min) {
            arr[i] = static_cast<int>(avg);
        }
        std::cout << arr[i] << " ";
    }

    delete[] arr;

    std::cout << "\nFinished successfuly";
}