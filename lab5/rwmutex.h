#pragma once
#include <windows.h>
#include <stdexcept>

class RWMutex {
private:
    volatile LONG readerCount;
    volatile LONG readerWait; 
    HANDLE writerSem;         
    HANDLE readerSem;         
    HANDLE wMutex;            

    static const LONG RWMUTEX_MAX_READERS = 1 << 30; // 1 миллиард

public:
    RWMutex() : readerCount(0), readerWait(0) {
        writerSem = CreateSemaphoreA(NULL, 0, MAXLONG, NULL);
        readerSem = CreateSemaphoreA(NULL, 0, MAXLONG, NULL);
        wMutex = CreateMutexA(NULL, FALSE, NULL);

        if (!writerSem || !readerSem || !wMutex) {
            throw std::runtime_error("Ошибка создания объектов ядра");
        }
    }

    ~RWMutex() {
        CloseHandle(writerSem);
        CloseHandle(readerSem);
        CloseHandle(wMutex);
    }

    void RLock() {
        if (InterlockedIncrement(&readerCount) < 0) {
            WaitForSingleObject(readerSem, INFINITE);
        }
    }

    void RUnlock() {
        if (InterlockedDecrement(&readerCount) < 0) {
            if (InterlockedDecrement(&readerWait) == 0) {
                ReleaseSemaphore(writerSem, 1, NULL);
            }
        }
    }

    void Lock() {
        WaitForSingleObject(wMutex, INFINITE);
        LONG r = InterlockedExchangeAdd(&readerCount, -RWMUTEX_MAX_READERS);
        if (r != 0) {
            if (InterlockedExchangeAdd(&readerWait, r) + r != 0) {
                WaitForSingleObject(writerSem, INFINITE);
            }
        }
    }

    void Unlock() {
        LONG r = InterlockedExchangeAdd(&readerCount, RWMUTEX_MAX_READERS) + RWMUTEX_MAX_READERS;
        if (r > 0) {
            ReleaseSemaphore(readerSem, r, NULL);
        }
        ReleaseMutex(wMutex);
    }
};