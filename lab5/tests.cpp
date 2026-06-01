#include <windows.h>
#include <iostream>
#include <vector>
#include "common.h"
#include "rwmutex.h"

void UpdateMax(volatile LONG* max_val, LONG current) {
    LONG prev_max = *max_val;
    while (current > prev_max) {
        LONG actual_prev = InterlockedCompareExchange(max_val, current, prev_max);
        if (actual_prev == prev_max) break;
        prev_max = actual_prev;
    }
}

// =========================================================
// ТЕСТ 1: Проверка возможности параллельного чтения
// =========================================================
struct T1_Data {
    RWMutex* rw;
    volatile LONG active_readers;
    volatile LONG max_readers;
};

DWORD WINAPI T1_ReaderThread(LPVOID lpParam) {
    T1_Data* data = (T1_Data*)lpParam;
    data->rw->RLock();

    LONG current = InterlockedIncrement(&data->active_readers);
    UpdateMax(&data->max_readers, current);

    Sleep(200);

    InterlockedDecrement(&data->active_readers);
    data->rw->RUnlock();
    return 0;
}

bool TestMultipleReaders() {
    std::cout << "[TEST] TestMultipleReaders... ";
    RWMutex rw;
    T1_Data data = { &rw, 0, 0 };
    const int NUM_THREADS = 5;
    std::vector<HANDLE> threads(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = CreateThread(NULL, 0, T1_ReaderThread, &data, 0, NULL);
    }

    WaitForMultipleObjects(NUM_THREADS, threads.data(), TRUE, INFINITE);
    for (HANDLE h : threads) CloseHandle(h);

    if (data.max_readers == NUM_THREADS) {
        std::cout << "PASSED\n";
        return true;
    }
    else {
        std::cout << "FAILED (max concurrent readers: " << data.max_readers << ")\n";
        return false;
    }
}

// =========================================================
// ТЕСТ 2: Писатель блокирует читателей
// =========================================================
struct T2_Data {
    RWMutex* rw;
    volatile LONG reader_acquired;
};

DWORD WINAPI T2_ReaderThread(LPVOID lpParam) {
    T2_Data* data = (T2_Data*)lpParam;
    data->rw->RLock();
    InterlockedExchange(&data->reader_acquired, 1);
    data->rw->RUnlock();
    return 0;
}

bool TestWriterBlocksReader() {
    std::cout << "[TEST] TestWriterBlocksReader... ";
    RWMutex rw;
    T2_Data data = { &rw, 0 };

    rw.Lock(); // Эксклюзивная блокировка (Писатель)

    HANDLE hReader = CreateThread(NULL, 0, T2_ReaderThread, &data, 0, NULL);
    Sleep(200); // Даем потоку-читателю время на попытку захвата

    LONG acquired_while_locked = InterlockedCompareExchange(&data.reader_acquired, 0, 0);

    rw.Unlock(); // Отпускаем блокировку
    WaitForSingleObject(hReader, INFINITE);
    CloseHandle(hReader);

    LONG acquired_after_unlock = InterlockedCompareExchange(&data.reader_acquired, 0, 0);

    if (acquired_while_locked == 0 && acquired_after_unlock == 1) {
        std::cout << "PASSED\n";
        return true;
    }
    else {
        std::cout << "FAILED\n";
        return false;
    }
}

// =========================================================
// ТЕСТ 3: Писатель блокирует другого писателя
// =========================================================
struct T3_Data {
    RWMutex* rw;
    volatile LONG writer2_acquired;
};

DWORD WINAPI T3_WriterThread(LPVOID lpParam) {
    T3_Data* data = (T3_Data*)lpParam;
    data->rw->Lock();
    InterlockedExchange(&data->writer2_acquired, 1);
    data->rw->Unlock();
    return 0;
}

bool TestWriterBlocksWriter() {
    std::cout << "[TEST] TestWriterBlocksWriter... ";
    RWMutex rw;
    T3_Data data = { &rw, 0 };

    rw.Lock();

    HANDLE hWriter2 = CreateThread(NULL, 0, T3_WriterThread, &data, 0, NULL);
    Sleep(200);

    LONG acquired_while_locked = InterlockedCompareExchange(&data.writer2_acquired, 0, 0);

    rw.Unlock();
    WaitForSingleObject(hWriter2, INFINITE);
    CloseHandle(hWriter2);

    LONG acquired_after_unlock = InterlockedCompareExchange(&data.writer2_acquired, 0, 0);

    if (acquired_while_locked == 0 && acquired_after_unlock == 1) {
        std::cout << "PASSED\n";
        return true;
    }
    else {
        std::cout << "FAILED\n";
        return false;
    }
}

// =========================================================
// ТЕСТ 4: Стресс-тест данных при наличии гонки (Race condition)
// =========================================================
struct T4_Data {
    RWMutex* rw;
    int shared_data;
    int increments;
};

DWORD WINAPI T4_WriterThread(LPVOID lpParam) {
    T4_Data* data = (T4_Data*)lpParam;
    for (int i = 0; i < data->increments; ++i) {
        data->rw->Lock();
        int temp = data->shared_data;
        Sleep(0);
        data->shared_data = temp + 1;
        data->rw->Unlock();
    }
    return 0;
}

DWORD WINAPI T4_ReaderThread(LPVOID lpParam) {
    T4_Data* data = (T4_Data*)lpParam;
    for (int i = 0; i < data->increments; ++i) {
        data->rw->RLock();
        volatile int read_val = data->shared_data;
        (void)read_val;
        data->rw->RUnlock();
        Sleep(0);
    }
    return 0;
}

bool TestStress() {
    std::cout << "[TEST] TestStress... ";
    RWMutex rw;
    T4_Data data = { &rw, 0, 1000 };

    const int NUM_WRITERS = 10;
    const int NUM_READERS = 10;
    std::vector<HANDLE> threads;

    for (int i = 0; i < NUM_WRITERS; ++i)
        threads.push_back(CreateThread(NULL, 0, T4_WriterThread, &data, 0, NULL));

    for (int i = 0; i < NUM_READERS; ++i)
        threads.push_back(CreateThread(NULL, 0, T4_ReaderThread, &data, 0, NULL));

    WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);
    for (HANDLE h : threads) CloseHandle(h);

    int expected = NUM_WRITERS * data.increments;
    if (data.shared_data == expected) {
        std::cout << "PASSED (Counter: " << data.shared_data << ")\n";
        return true;
    }
    else {
        std::cout << "FAILED (Counter: " << data.shared_data << " != " << expected << ")\n";
        return false;
    }
}

// =========================================================
// ТОЧКА ВХОДА
// =========================================================
int main() {
    std::cout << "Starting RWMutex tests (WinAPI)...\n";

    bool success = true;
    try {
        success &= TestMultipleReaders();
        success &= TestWriterBlocksReader();
        success &= TestWriterBlocksWriter();
        success &= TestStress();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    if (success) {
        std::cout << "All tests finished successfully.\n";
        return EXIT_SUCCESS;
    }
    else {
        std::cout << "Some tests failed.\n";
        return EXIT_FAILURE;
    }
}