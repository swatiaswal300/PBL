#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <climits>
#include <limits>

using namespace std;

const int MAX_LOCATIONS = 50;
const int INF = INT_MAX / 4;

// ======================================================
// COMMON INPUT FUNCTIONS
// ======================================================

int readInt(string message) {
    int value;

    while (true) {
        cout << message;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readString(string message) {
    string value;

    cout << message;
    getline(cin >> ws, value);

    return value;
}
