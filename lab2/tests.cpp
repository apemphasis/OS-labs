#include <iostream>
#include <cassert>
#include "threads.h"

void TestMinMax_1() {
    int test_data[] = { 10, -5, 20, 0, 8 };
    Array test_arr = { test_data, 5 };
    MinMax req = { test_arr, 0, 0 };

    min_max(&req); 

    assert(req.min == -5 && "Min should be -5");
    assert(req.max == 20 && "Max should be 20");
    std::cout << "[OK] TestMinMax Passed!\n";
}

void TestMinMax_2() {
    int test_data[] = { 10 };
    Array test_arr = { test_data, 1 };
    MinMax req = { test_arr, 0, 0 };

    min_max(&req); 

    assert(req.min == 10 && "Min should be 10");
    assert(req.max == 10 && "Max should be 10");
    std::cout << "[OK] TestMinMax Passed!\n";
}

void TestMinMax_3() {
    int test_data[] = { 10, 10, -4, 10 };
    Array test_arr = { test_data, 4 };
    MinMax req = { test_arr, 0, 0 };

    min_max(&req);

    assert(req.min == -4 && "Min should be -4");
    assert(req.max == 10 && "Max should be 10");
    std::cout << "[OK] TestMinMax Passed!\n";
}

void TestMinMax_4() {
    int test_data[] = { 10, 10, 10, 10 };
    Array test_arr = { test_data, 4 };
    MinMax req = { test_arr, 0, 0 };

    min_max(&req);

    assert(req.min == 10 && "Min should be 10");
    assert(req.max == 10 && "Max should be 10");
    std::cout << "[OK] TestMinMax Passed!\n";
}

void TestAverage_1() {
    int test_data[] = { 2, 4, 6, 8 };
    Array test_arr = { test_data, 4 };
    Average req = { test_arr, 0.0 };

    average(&req);

    assert(req.avg == 5.0 && "Average should be 5.0");
    std::cout << "[OK] TestAverage Passed!\n";
}

void TestAverage_2() {
    int test_data[] = { 2 };
    Array test_arr = { test_data, 1 };
    Average req = { test_arr, 0.0 };

    average(&req);

    assert(req.avg == 2.0 && "Average should be 2.0");
    std::cout << "[OK] TestAverage Passed!\n";
}

void TestAverage_3() {
    int test_data[] = { 2, 2, 2 };
    Array test_arr = { test_data, 3 };
    Average req = { test_arr, 0.0 };

    average(&req);

    assert(req.avg == 2.0 && "Average should be 2.0");
    std::cout << "[OK] TestAverage Passed!\n";
}

void TestComplex() {
    int test_data[] = { 3, 14, -15, 92, 14 };
    Array test_arr = { test_data, 5 };
    MinMax minmax_req = { test_arr, 0, 0 };
    Average avg_req = { test_arr, 0.0 };

    min_max(&minmax_req);
    average(&avg_req);

    assert(minmax_req.min == -15 && "Min should be -15");
    assert(minmax_req.max == 92 && "Max should be 92");
    assert(avg_req.avg == 21.6 && "Average should be 21.6");
}

int main() {
    std::cout << "--- Running Unit Tests ---\n";
    TestMinMax_1();
    TestMinMax_2();
    TestMinMax_3();
    TestMinMax_4();
    TestAverage_1();
    TestAverage_2();
    TestAverage_3();
    TestComplex();
    std::cout << "--- All tests passed! ---\n";
    return 0;
}