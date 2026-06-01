#include <cassert>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include "utilities.h";

void test_toCharStr() {
    // 1. Пустая строка
    std::string empty;
    char* p = toCharStr(empty);
    assert(p != nullptr);
    assert(strlen(p) == 0);
    assert(p[0] == '\0');
    delete[] p;

    // 2. Непустая строка
    std::string testStr = "Hello, world!";
    p = toCharStr(testStr);
    assert(p != nullptr);
    assert(strcmp(p, testStr.c_str()) == 0);
    assert(strlen(p) == testStr.length());
    delete[] p;
}

void test_writefile() {
    const char* filename = "test_writefile.tmp";
    // Создаём файл с начальным содержимым
    {
        std::ofstream init(filename, std::ios::trunc);
        init << "Initial-data---";
        init.close();
    }

    // Подготавливаем буфер размером 20 байт
    char buffer[21] = "12345678901234567890"; // ровно 20 символов
    std::ofstream fout(filename, std::ios::app | std::ios::binary);
    assert(fout.is_open());

    // Нормальная запись (размер >=20)
    writefile(fout, buffer, sizeof(buffer));
    fout.close();

    // Проверяем: в конец файла должно дописаться 20 байт
    std::ifstream fin(filename, std::ios::binary | std::ios::ate);
    assert(fin.is_open());
    size_t fileSize = fin.tellg();
    fin.close();
    // Исходная фраза "Initial data. " имеет длину 15 байт (точка и пробел)
    // После записи 20 байт размер должен стать 35
    assert(fileSize == 35);

    // Проверка исключения при size < 20
    char smallBuffer[10] = "small";
    std::ofstream fout2(filename, std::ios::app | std::ios::binary);
    assert(fout2.is_open());
    try {
        writefile(fout2, smallBuffer, sizeof(smallBuffer));
        assert(false); // Сюда не должны попасть
    }
    catch (const std::invalid_argument& e) {
        assert(true);
    }
    catch (...) {
        assert(false);
    }
    fout2.close();

    // Очистка
    std::remove(filename);
}

void test_readfile() {
    const char* filename = "test_readfile.tmp";
    // Создаём файл с 25 байтами
    {
        std::ofstream fout(filename, std::ios::binary);
        fout << "1234567890123456789012345"; // 25 символов
    }

    // Чтение в буфер размера 20
    char buffer[20];
    std::ifstream fin(filename, std::ios::binary);
    assert(fin.is_open());

    // 1. Нормальное чтение 20 байт
    int bytes = readfile(fin, buffer, sizeof(buffer));
    assert(bytes == 20);
    assert(memcmp(buffer, "12345678901234567890", 20) == 0);

    // 2. Чтение оставшихся 5 байт (ещё один вызов)
    bytes = readfile(fin, buffer, sizeof(buffer));
    assert(bytes == 5);                 // прочитано только 5
    assert(memcmp(buffer, "12345", 5) == 0);
    // Проверяем, что остальная часть буфера (с 5 по 19) не была изменена
    // Но поскольку буфер старый, то байты после 5 могут содержать мусор.
    // Корректнее: перед вторым чтением заполним буфер известными значениями.
    // Повторим тест более аккуратно:
    fin.clear(); // сбросим eof
    fin.seekg(0); // вернёмся в начало

    char testBuf[20];
    memset(testBuf, 0xAA, sizeof(testBuf)); // заполняем паттерном
    bytes = readfile(fin, testBuf, sizeof(testBuf));
    assert(bytes == 20);
    assert(memcmp(testBuf, "12345678901234567890", 20) == 0);

    // Теперь читаем остаток файла (должно быть 5 байт)
    memset(testBuf, 0xAA, sizeof(testBuf));
    bytes = readfile(fin, testBuf, sizeof(testBuf));
    assert(bytes == 5);
    assert(memcmp(testBuf, "12345", 5) == 0);
    // Байты с 5 по 19 должны остаться 0xAA
    for (int i = 5; i < 20; ++i) {
        assert(testBuf[i] == static_cast<char>(0xAA));
    }
    fin.close();

    // 3. Чтение из пустого файла
    {
        std::ofstream emptyFile(filename, std::ios::trunc);
        emptyFile.close();
        std::ifstream finEmpty(filename, std::ios::binary);
        char buf[20];
        memset(buf, 0xBB, sizeof(buf));
        int bytesRead = readfile(finEmpty, buf, sizeof(buf));
        assert(bytesRead == 0);
        // Буфер не должен измениться (ничего не прочитано)
        for (int i = 0; i < 20; ++i) {
            assert(buf[i] == static_cast<char>(0xBB));
        }
        finEmpty.close();
    }

    // 4. Исключение при size < 20
    char smallBuf[10];
    std::ifstream finSmall(filename, std::ios::binary);
    try {
        readfile(finSmall, smallBuf, sizeof(smallBuf));
        assert(false);
    }
    catch (const std::invalid_argument& e) {
        assert(true);
    }
    catch (...) {
        assert(false);
    }
    finSmall.close();

    std::remove(filename);
}

int main() {
    test_toCharStr();
    test_writefile();
    test_readfile();
    return 0;
}