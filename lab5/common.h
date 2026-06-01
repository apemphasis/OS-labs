#pragma once
#include <windows.h>
#include <stdexcept>


struct employee {
    int num;         
    char name[10];   
    double hours;    
};

// типы команд от клиента серверу
enum Command {
    CMD_GET_FOR_READ,
    CMD_RELEASE_READ,
    CMD_GET_FOR_MODIFY,
    CMD_SAVE_MODIFY,
    CMD_RELEASE_MODIFY,
    CMD_QUIT
};

// сообщение-запрос
struct Request {
    Command cmd;
    int id;
    employee emp;
};

// сообщение-ответ
struct Response {
    bool success;
    char error_msg[50];
    employee emp;
};

const char* PIPE_NAME = "\\\\.\\pipe\\lab_named_pipe";