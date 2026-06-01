#pragma once
#include <stdexcept>

char* toCharStr(std::string str) {
    char* cmdLine = new char[str.length() + 1];
    strcpy(cmdLine, str.c_str());
    return cmdLine;
}

void writefile(std::ofstream& fout, char* buff, size_t size) {
    if (size < 20) {
        throw std::invalid_argument("Buffer size must be at least 20 bytes");
    }
    fout.seekp(0, std::ios::end);
    fout.write(buff, 20);
    fout.flush();
}

int readfile(std::ifstream& fin, char* buff, size_t size) {
    if (size < 20) {
        throw std::invalid_argument("Buffer size must be at least 20 bytes");
    }
    fin.read(buff, 20);
    fin.clear();
    return fin.gcount();
}