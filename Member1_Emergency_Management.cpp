#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <limits>
using namespace std;

struct Emergency {
    int id;
    string victimName;
    string type;
    int severity;
    string location;
};

struct CompareEmergency {
    bool operator()(Emergency a, Emergency b) {
        if (a.severity != b.severity)
            return a.severity < b.severity;

        return a.id > b.id;
    }
};

class EmergencyManager {
private:
    vector<Emergency> emergencies;
    int nextId = 1;

public:
    EmergencyManager() {
        loadEmergencies();
    }

    void addEmergency() {
        Emergency e;
        e.id = nextId++;

        cout << "Enter victim name: ";
        getline(cin >> ws, e.victimName);

        cout << "Enter emergency type: ";
        getline(cin, e.type);

        cout << "Enter location: ";
        getline(cin, e.location);

        do {
            cout << "Enter severity (3=High, 2=Medium, 1=Low): ";
            cin >> e.severity;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                e.severity = 0;
            }

            if (e.severity < 1 || e.severity > 3)
                cout << "Enter a valid severity!\n";
        } while (e.severity < 1 || e.severity > 3);

        emergencies.push_back(e);
        saveEmergencies();

        cout << "\nEmergency added successfully!\n";
        cout << "Emergency ID: " << e.id << "\n";
    }

    void displayEmergencies() {
        if (emergencies.empty()) {
            cout << "\nNo emergencies available.\n";
            return;
        }

        priority_queue<Emergency, vector<Emergency>, CompareEmergency> pq;
        for (Emergency e : emergencies)
            pq.push(e);

        cout << "\n===== EMERGENCY RECORDS =====\n";
        while (!pq.empty()) {
            Emergency e = pq.top();
            pq.pop();

            cout << "\nID: " << e.id;
            cout << "\nVictim Name: " << e.victimName;
            cout << "\nEmergency Type: " << e.type;
            cout << "\nSeverity: " << e.severity;
            cout << "\nLocation: " << e.location;
            cout << "\n--------------------------\n";
        }
    }

    void searchEmergency() {
        int id;
        cout << "Enter Emergency ID to search: ";
        cin >> id;

        for (const Emergency& e : emergencies) {
            if (e.id == id) {
                cout << "\nEmergency Found!\n";
                cout << "ID: " << e.id << "\n";
                cout << "Victim: " << e.victimName << "\n";
                cout << "Type: " << e.type << "\n";
                cout << "Severity: " << e.severity << "\n";
                cout << "Location: " << e.location << "\n";
                return;
            }
        }
        cout << "Emergency ID not found.\n";
    }

    void deleteEmergency() {
        int id;
        cout << "Enter Emergency ID to delete: ";
        cin >> id;

        for (int i = 0; i < static_cast<int>(emergencies.size()); i++) {
            if (emergencies[i].id == id) {
                emergencies.erase(emergencies.begin() + i);
                saveEmergencies();
                cout << "Emergency deleted successfully!\n";
                return;
            }
        }
        cout << "Emergency ID not found.\n";
    }

    void updateEmergency() {
        int id;
        cout << "Enter Emergency ID to update: ";
        cin >> id;

        for (Emergency& e : emergencies) {
            if (e.id == id) {
                cout << "Enter new victim name: ";
                getline(cin >> ws, e.victimName);

                cout << "Enter new emergency type: ";
                getline(cin, e.type);

                cout << "Enter new location: ";
                getline(cin, e.location);

                int severity;
                do {
                    cout << "Enter new severity (3=High, 2=Medium, 1=Low): ";
                    cin >> severity;

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        severity = 0;
                    }

                    if (severity < 1 || severity > 3)
                        cout << "Enter a valid severity!\n";
                } while (severity < 1 || severity > 3);

                e.severity = severity;
                saveEmergencies();
                cout << "Emergency updated successfully!\n";
                return;
            }
        }
        cout << "Emergency ID not found.\n";
    }

    void saveEmergencies() {
        ofstream file("emergencies.txt");
        if (!file) {
            cout << "Unable to save emergency records.\n";
            return;
        }

        for (const Emergency& e : emergencies) {
            file << e.id << "|"
                 << e.victimName << "|"
                 << e.type << "|"
                 << e.severity << "|"
                 << e.location << "\n";
        }
        file.close();
    }

    void loadEmergencies() {
        ifstream file("emergencies.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            Emergency e;
            string idText, severityText;

            getline(ss, idText, '|');
            getline(ss, e.victimName, '|');
            getline(ss, e.type, '|');
            getline(ss, severityText, '|');
            getline(ss, e.location, '|');

            try {
                e.id = stoi(idText);
                e.severity = stoi(severityText);

                if (e.severity >= 1 && e.severity <= 3) {
                    emergencies.push_back(e);
                    if (e.id >= nextId)
                        nextId = e.id + 1;
                }
            } catch (...) {
                cout << "Invalid record skipped.\n";
            }
        }
        file.close();
    }
};

int main() {
    EmergencyManager manager;
    int choice;

    do {
        cout << "\n===== MEMBER 1: EMERGENCY MANAGEMENT =====\n";
        cout << "1. Add Emergency\n";
        cout << "2. Display Emergencies\n";
        cout << "3. Search Emergency by ID\n";
        cout << "4. Delete Emergency\n";
        cout << "5. Update Emergency\n";
        cout << "0. Exit\n";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a valid number.\n";
            continue;
        }

        switch (choice) {
            case 1: manager.addEmergency(); break;
            case 2: manager.displayEmergencies(); break;
            case 3: manager.searchEmergency(); break;
            case 4: manager.deleteEmergency(); break;
            case 5: manager.updateEmergency(); break;
            case 0: cout << "Exiting Emergency Management.\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}
