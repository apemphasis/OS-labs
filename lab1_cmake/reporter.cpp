#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: Reporter <binary file> <report file> <hourly rate>" << endl;
        return 1;
    }

    const char* binFile = argv[1];
    const char* reportFile = argv[2];
    double rate = atof(argv[3]);

    if (rate <= 0) {
        cerr << "Hourly rate must be positive." << endl;
        return 1;
    }

    ifstream in(binFile, ios::binary);
    if (!in) {
        cerr << "Error: Cannot open binary file '" << binFile << "'" << endl;
        return 1;
    }

    ofstream out(reportFile);
    if (!out) {
        cerr << "Error: Cannot create report file '" << reportFile << "'" << endl;
        return 1;
    }

    out << "Report on file \"" << binFile << "\"\n";
    out << "------------------------------------------------\n";
    out << left << setw(15) << "Employee ID"
        << setw(15) << "Name"
        << setw(10) << "Hours"
        << "Salary\n";
    out << "------------------------------------------------\n";

    employee emp{};
    int recordCount = 0;
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        double salary = emp.hours * rate;
        out << left << setw(15) << emp.num
            << setw(15) << emp.name
            << setw(10) << fixed << setprecision(2) << emp.hours
            << fixed << setprecision(2) << salary << "\n";
        recordCount++;
    }

    out << "------------------------------------------------\n";
    out << "Total records: " << recordCount << "\n";

    in.close();
    out.close();

    cout << "Reporter: Successfully created report with " << recordCount << " records." << endl;
    return 0;
}