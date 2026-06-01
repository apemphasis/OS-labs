#include <windows.h>
#include <iostream>
#include <fstream>
#include <limits>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: Creator <binary file name> <number of records>" << endl;
        return 1;
    }

    const char* filename = argv[1];
    const int recordCount = atoi(argv[2]);

    if (recordCount <= 0) {
        cerr << "Number of records must be positive." << endl;
        return 1;
    }

    ofstream out(filename, ios::binary);
    if (!out) {
        cerr << "Failed to create file." << endl;
        return 1;
    }

    for (int i = 0; i < recordCount; ++i) {
        employee emp{};
        cout << "\nEnter employee #" << i + 1 << ":\n";

        cout << "ID (integer): ";
        while (!(cin >> emp.num)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter an integer: ";
        }

        cout << "Name (max 9 chars): ";
        cin >> ws;
        cin.getline(emp.name, 10);

        cout << "Hours: ";
        while (!(cin >> emp.hours) || emp.hours < 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a non-negative number: ";
        }

        out.write(reinterpret_cast<char*>(&emp), sizeof(emp));
        if (!out) {
            cerr << "Error writing to file." << endl;
            return 1;
        }
    }

    out.close();
    cout << "\nCreator: Successfully created " << recordCount << " records in " << filename << endl;
    return 0;
}