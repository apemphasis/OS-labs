#pragma once
#include <windows.h>
#include <string>

struct employee {
    int num;
    char name[10];
    double hours;
};

bool comparator(const employee& a, const employee& b) {
    return a.num < b.num;
}

std::string GetWindowsErrorText(DWORD errorCode) {
    if (errorCode == 0) return "No error";
    
    LPTSTR messageBuffer = nullptr; 
    
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, 
        errorCode, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
        (LPTSTR)&messageBuffer, 
        0, 
        NULL
    );
        
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}