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

// ======================================================
// MEMBER 1: EMERGENCY MANAGEMENT
// PRIORITY QUEUE + FCFS
// ======================================================

struct Emergency {
    int id;
    string victimName;
    string location;
    int priority;
    string requiredType;
    int sequence;
};

struct EmergencyCompare {
    bool operator()(const Emergency& a,
                    const Emergency& b) const {

        // Higher priority comes first.
        if (a.priority != b.priority) {
            return a.priority < b.priority;
        }

        // Earlier registration comes first for equal priority.
        return a.sequence > b.sequence;
    }
};

class EmergencyManager {
private:
    priority_queue<Emergency,
                   vector<Emergency>,
                   EmergencyCompare> emergencies;

    int nextId = 1;
    int nextSequence = 1;

public:

    EmergencyManager() {
        loadEmergencies();
    }

    void addEmergency() {
        Emergency e;

        e.id = nextId++;

        e.victimName = readString("Enter victim name: ");
        e.location = readString("Enter emergency location: ");

        do {
            e.priority = readInt(
                "Enter priority (3=High, 2=Medium, 1=Low): "
            );

            if (e.priority < 1 || e.priority > 3) {
                cout << "Priority must be 1, 2 or 3.\n";
            }

        } while (e.priority < 1 || e.priority > 3);

        e.requiredType = readString(
            "Required resource (Ambulance/Medical/Rescue): "
        );

        e.sequence = nextSequence++;

        emergencies.push(e);

        saveEmergencies();

        cout << "\nEmergency registered successfully!\n";
        cout << "Emergency ID: " << e.id << "\n";
    }

    bool getNextEmergency(Emergency& e) {
        if (emergencies.empty()) {
            return false;
        }

        e = emergencies.top();
        return true;
    }

    void removeNextEmergency() {
        if (!emergencies.empty()) {
            emergencies.pop();
            saveEmergencies();
        }
    }

    void displayEmergencies() {
        if (emergencies.empty()) {
            cout << "\nNo pending emergencies.\n";
            return;
        }

        priority_queue<Emergency,
                       vector<Emergency>,
                       EmergencyCompare> temp = emergencies;

        cout << "\n========== PENDING EMERGENCIES ==========\n";

        while (!temp.empty()) {
            Emergency e = temp.top();
            temp.pop();

            cout << "\nEmergency ID: " << e.id;
            cout << "\nVictim Name: " << e.victimName;
            cout << "\nLocation: " << e.location;
            cout << "\nPriority: " << e.priority;
            cout << "\nRequired Resource: " << e.requiredType;
            cout << "\n----------------------------------------\n";
        }
    }

    void saveEmergencies() {
        ofstream file("emergencies.txt");

        if (!file.is_open()) {
            cout << "Could not save emergency records.\n";
            return;
        }

        auto temp = emergencies;

        while (!temp.empty()) {
            Emergency e = temp.top();
            temp.pop();

            file << e.id << "|"
                 << e.victimName << "|"
                 << e.location << "|"
                 << e.priority << "|"
                 << e.requiredType << "|"
                 << e.sequence << "\n";
        }

        file.close();
    }

    void loadEmergencies() {
        ifstream file("emergencies.txt");

        if (!file.is_open()) {
            return;
        }

        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string field;
            Emergency e;

            try {
                getline(ss, field, '|');
                e.id = stoi(field);

                getline(ss, e.victimName, '|');
                getline(ss, e.location, '|');

                getline(ss, field, '|');
                e.priority = stoi(field);

                getline(ss, e.requiredType, '|');

                getline(ss, field, '|');
                e.sequence = stoi(field);

                if (e.priority < 1 || e.priority > 3) {
                    continue;
                }

                emergencies.push(e);

                if (e.id >= nextId) {
                    nextId = e.id + 1;
                }

                if (e.sequence >= nextSequence) {
                    nextSequence = e.sequence + 1;
                }
            }
            catch (...) {
                cout << "Skipping an invalid emergency record.\n";
            }
        }

        file.close();
    }
};

// ======================================================
// MEMBER 3: GRAPH AND DIJKSTRA'S SHORTEST PATH
// ======================================================

struct PathResult {
    vector<int> dist;
    vector<int> parent;
};

class Graph {
private:
    string locations[MAX_LOCATIONS];

    int distance[MAX_LOCATIONS][MAX_LOCATIONS];

    int count;

public:

    Graph() {
        count = 0;

        for (int i = 0; i < MAX_LOCATIONS; i++) {
            for (int j = 0; j < MAX_LOCATIONS; j++) {
                distance[i][j] = (i == j) ? 0 : INF;
            }
        }

        loadGraph();
    }

    int findLocation(string name) {
        for (int i = 0; i < count; i++) {
            if (locations[i] == name) {
                return i;
            }
        }

        return -1;
    }

    void addLocation() {
        if (count >= MAX_LOCATIONS) {
            cout << "Maximum location limit reached.\n";
            return;
        }

        string name = readString("Enter location name: ");

        if (name.empty() || findLocation(name) != -1) {
            cout << "Invalid or duplicate location.\n";
            return;
        }

        locations[count] = name;
        count++;

        saveGraph();

        cout << "Location added successfully!\n";
    }

    void displayLocations() {
        if (count == 0) {
            cout << "\nNo locations available.\n";
            return;
        }

        cout << "\n========== LOCATIONS ==========\n";

        for (int i = 0; i < count; i++) {
            cout << i << " : " << locations[i] << "\n";
        }
    }

    void addRoad() {
        if (count < 2) {
            cout << "Add at least two locations first.\n";
            return;
        }

        string source = readString("Enter starting location: ");
        string destination = readString("Enter destination location: ");

        int u = findLocation(source);
        int v = findLocation(destination);

        if (u == -1 || v == -1 || u == v) {
            cout << "Invalid locations.\n";
            return;
        }

        int weight = readInt("Enter road distance: ");

        if (weight <= 0) {
            cout << "Distance must be positive.\n";
            return;
        }

        // Undirected weighted graph.
        distance[u][v] = weight;
        distance[v][u] = weight;

        saveGraph();

        cout << "Road added successfully!\n";
    }

    void displayRoads() {
        cout << "\n========== ROAD CONNECTIONS ==========\n";

        bool found = false;

        for (int i = 0; i < count; i++) {
            for (int j = i + 1; j < count; j++) {
                if (distance[i][j] != INF) {
                    cout << locations[i] << " <--> "
                         << locations[j]
                         << " : " << distance[i][j] << "\n";

                    found = true;
                }
            }
        }

        if (!found) {
            cout << "No roads have been added.\n";
        }
    }

    // DIJKSTRA'S ALGORITHM

    PathResult dijkstra(int source) {
        PathResult result;

        result.dist.assign(count, INF);
        result.parent.assign(count, -1);

        vector<bool> visited(count, false);

        result.dist[source] = 0;

        for (int i = 0; i < count; i++) {
            int u = -1;

            // Find the unvisited vertex with minimum distance.
            for (int j = 0; j < count; j++) {
                if (!visited[j] &&
                    (u == -1 ||
                     result.dist[j] < result.dist[u])) {

                    u = j;
                }
            }

            // Stop if no more reachable vertices exist.
            if (u == -1 || result.dist[u] == INF) {
                break;
            }

            visited[u] = true;

            // Update distances of adjacent vertices.
            for (int v = 0; v < count; v++) {
                if (!visited[v] &&
                    distance[u][v] != INF &&
                    result.dist[u] <= INF - distance[u][v] &&
                    result.dist[u] + distance[u][v] < result.dist[v]) {

                    result.dist[v] =
                        result.dist[u] + distance[u][v];

                    result.parent[v] = u;
                }
            }
        }

        return result;
    }

    // Display the complete shortest route.

    void displayPath(int destination,
                     const vector<int>& parent) {

        vector<int> path;

        int current = destination;

        while (current != -1) {
            path.push_back(current);
            current = parent[current];
        }

        cout << "Route: ";

        for (int i = (int)path.size() - 1; i >= 0; i--) {
            cout << locations[path[i]];

            if (i != 0) {
                cout << " -> ";
            }
        }

        cout << "\n";
    }

    void displayShortestDistances() {
        if (count == 0) {
            cout << "No locations available.\n";
            return;
        }

        string sourceName = readString("Enter source location: ");

        int source = findLocation(sourceName);

        if (source == -1) {
            cout << "Source location not found.\n";
            return;
        }

        PathResult result = dijkstra(source);

        cout << "\n========== SHORTEST DISTANCES ==========\n";

        for (int i = 0; i < count; i++) {
            cout << sourceName << " -> " << locations[i] << " : ";

            if (result.dist[i] == INF) {
                cout << "Unreachable\n";
            }
            else {
                cout << result.dist[i] << "\n";

                if (i != source) {
                    displayPath(i, result.parent);
                }
            }
        }
    }

    void saveGraph() {
        ofstream locationFile("locations.txt");

        for (int i = 0; i < count; i++) {
            locationFile << locations[i] << "\n";
        }

        locationFile.close();

        ofstream roadFile("roads.txt");

        for (int i = 0; i < count; i++) {
            for (int j = i + 1; j < count; j++) {
                if (distance[i][j] != INF) {
                    roadFile << i << "|"
                             << j << "|"
                             << distance[i][j] << "\n";
                }
            }
        }

        roadFile.close();
    }

    void loadGraph() {
        ifstream locationFile("locations.txt");

        if (locationFile.is_open()) {
            string name;

            while (getline(locationFile, name)) {
                if (count < MAX_LOCATIONS && !name.empty()) {
                    locations[count++] = name;
                }
            }

            locationFile.close();
        }

        ifstream roadFile("roads.txt");

        if (roadFile.is_open()) {
            string line;

            while (getline(roadFile, line)) {
                stringstream ss(line);

                string a, b, w;

                getline(ss, a, '|');
                getline(ss, b, '|');
                getline(ss, w, '|');

                try {
                    int u = stoi(a);
                    int v = stoi(b);
                    int weight = stoi(w);

                    if (u >= 0 && u < count &&
                        v >= 0 && v < count &&
                        u != v && weight > 0) {

                        distance[u][v] = weight;
                        distance[v][u] = weight;
                    }
                }
                catch (...) {
                    cout << "Skipping an invalid road record.\n";
                }
            }

            roadFile.close();
        }
    }
};

// ======================================================
// MEMBER 2: RESOURCE MANAGEMENT
// LINKED LIST + INHERITANCE
// ======================================================

class Resource {
protected:
    int id;
    string name;
    string location;
    bool available;

public:

    Resource(int i, string n, string loc) {
        id = i;
        name = n;
        location = loc;
        available = true;
    }

    virtual string getType() const = 0;

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    string getLocation() const {
        return location;
    }

    bool isAvailable() const {
        return available;
    }

    void setAvailable(bool value) {
        available = value;
    }

    virtual ~Resource() {}
};

class Ambulance : public Resource {
public:
    Ambulance(int i, string n, string loc)
        : Resource(i, n, loc) {}

    string getType() const override {
        return "Ambulance";
    }
};

class MedicalTeam : public Resource {
public:
    MedicalTeam(int i, string n, string loc)
        : Resource(i, n, loc) {}

    string getType() const override {
        return "Medical";
    }
};

class RescueTeam : public Resource {
public:
    RescueTeam(int i, string n, string loc)
        : Resource(i, n, loc) {}

    string getType() const override {
        return "Rescue";
    }
};

struct ResourceNode {
    Resource* data;
    ResourceNode* next;

    ResourceNode(Resource* resource) {
        data = resource;
        next = nullptr;
    }
};

class ResourceManager {
private:
    ResourceNode* head;
    int nextResourceId;

    Resource* createResource(int id,
                             string name,
                             string location,
                             string type) {

        if (type == "Ambulance") {
            return new Ambulance(id, name, location);
        }

        if (type == "Medical") {
            return new MedicalTeam(id, name, location);
        }

        if (type == "Rescue") {
            return new RescueTeam(id, name, location);
        }

        return nullptr;
    }

public:

    ResourceManager() {
        head = nullptr;
        nextResourceId = 1;

        loadResources();
    }

    ~ResourceManager() {
        ResourceNode* current = head;

        while (current != nullptr) {
            ResourceNode* temp = current;
            current = current->next;

            delete temp->data;
            delete temp;
        }
    }

    void addResource() {
        string name = readString("Enter resource name: ");
        string location = readString("Enter resource location: ");

        cout << "Types: Ambulance, Medical, Rescue\n";

        string type = readString("Enter resource type: ");

        Resource* resource = createResource(
            nextResourceId, name, location, type
        );

        if (resource == nullptr) {
            cout << "Invalid resource type.\n";
            return;
        }

        ResourceNode* newNode = new ResourceNode(resource);

        // Insert at the beginning of linked list.
        newNode->next = head;
        head = newNode;

        nextResourceId++;

        saveResources();

        cout << "Resource registered successfully!\n";
        cout << "Resource ID: " << resource->getId() << "\n";
    }

    void displayResources() {
        if (head == nullptr) {
            cout << "\nNo resources registered.\n";
            return;
        }

        ResourceNode* current = head;

        cout << "\n========== RESOURCE LIST ==========\n";

        while (current != nullptr) {
            Resource* r = current->data;

            cout << "\nID: " << r->getId();
            cout << "\nName: " << r->getName();
            cout << "\nType: " << r->getType();
            cout << "\nLocation: " << r->getLocation();

            cout << "\nStatus: "
                 << (r->isAvailable() ? "Available" : "Allocated");

            cout << "\n-----------------------------------\n";

            current = current->next;
        }
    }

    bool allocateResource(const Emergency& e, Graph& graph) {
        int source = graph.findLocation(e.location);

        if (source == -1) {
            cout << "Emergency location is not registered in the graph.\n";
            return false;
        }

        PathResult result = graph.dijkstra(source);

        ResourceNode* current = head;

        Resource* bestResource = nullptr;
        int bestDistance = INF;
        int bestLocation = -1;

        while (current != nullptr) {
            Resource* r = current->data;

            if (r->isAvailable() &&
                r->getType() == e.requiredType) {

                int locationIndex = graph.findLocation(r->getLocation());

                if (locationIndex != -1 &&
                    result.dist[locationIndex] < bestDistance) {

                    bestDistance = result.dist[locationIndex];
                    bestResource = r;
                    bestLocation = locationIndex;
                }
            }

            current = current->next;
        }

        if (bestResource == nullptr || bestDistance == INF) {
            cout << "No reachable available matching resource found.\n";
            return false;
        }

        bestResource->setAvailable(false);

        saveResources();

        cout << "\n========== ALLOCATION SUCCESSFUL ==========\n";

        cout << "Emergency ID: " << e.id << "\n";
        cout << "Victim: " << e.victimName << "\n";
        cout << "Resource: " << bestResource->getName() << "\n";
        cout << "Resource Type: " << bestResource->getType() << "\n";
        cout << "Resource Location: " << bestResource->getLocation() << "\n";
        cout << "Shortest Distance: " << bestDistance << "\n";

        cout << "\n";

        // Show route from emergency to resource.
        graph.displayPath(bestLocation, result.parent);

        ofstream history("allocation_history.txt", ios::app);

        if (history.is_open()) {
            history << "Emergency ID: " << e.id
                    << " | Resource ID: " << bestResource->getId()
                    << " | Resource Name: " << bestResource->getName()
                    << " | Distance: " << bestDistance << "\n";

            history.close();
        }

        return true;
    }

    void releaseResource() {
        int id = readInt("Enter resource ID to release: ");

        ResourceNode* current = head;

        while (current != nullptr) {
            if (current->data->getId() == id) {
                if (current->data->isAvailable()) {
                    cout << "Resource is already available.\n";
                }
                else {
                    current->data->setAvailable(true);
                    saveResources();

                    cout << "Resource released successfully!\n";
                }

                return;
            }

            current = current->next;
        }

        cout << "Resource ID not found.\n";
    }

    void saveResources() {
        ofstream file("resources.txt");

        ResourceNode* current = head;

        while (current != nullptr) {
            Resource* r = current->data;

            file << r->getId() << "|"
                 << r->getName() << "|"
                 << r->getLocation() << "|"
                 << r->getType() << "|"
                 << r->isAvailable() << "\n";

            current = current->next;
        }

        file.close();
    }

    void loadResources() {
        ifstream file("resources.txt");

        if (!file.is_open()) {
            return;
        }

        string line;

        while (getline(file, line)) {
            stringstream ss(line);

            string idText, name, location, type, availableText;

            getline(ss, idText, '|');
            getline(ss, name, '|');
            getline(ss, location, '|');
            getline(ss, type, '|');
            getline(ss, availableText, '|');

            try {
                int id = stoi(idText);

                Resource* resource = createResource(
                    id, name, location, type
                );

                if (resource == nullptr) {
                    continue;
                }

                resource->setAvailable(availableText == "1");

                ResourceNode* newNode = new ResourceNode(resource);

                newNode->next = head;
                head = newNode;

                if (id >= nextResourceId) {
                    nextResourceId = id + 1;
                }
            }
            catch (...) {
                cout << "Skipping an invalid resource record.\n";
            }
        }

        file.close();
    }
};

// ======================================================
// MEMBER 4: MAIN SYSTEM INTEGRATION
// ======================================================

int main() {
    Graph graph;
    EmergencyManager emergencyManager;
    ResourceManager resourceManager;

    int choice;

    do {
        cout << "\n\n";
        cout << "============================================\n";
        cout << "     EMERGENCY RESOURCE ALLOCATION SYSTEM\n";
        cout << "============================================\n";

        cout << "\n--- EMERGENCY MANAGEMENT ---\n";
        cout << "1. Register Emergency\n";
        cout << "2. Display Pending Emergencies\n";

        cout << "\n--- RESOURCE MANAGEMENT ---\n";
        cout << "3. Add Resource\n";
        cout << "4. Display Resources\n";
        cout << "5. Release Resource\n";

        cout << "\n--- GRAPH MANAGEMENT ---\n";
        cout << "6. Add Location\n";
        cout << "7. Display Locations\n";
        cout << "8. Add Road\n";
        cout << "9. Display Roads\n";
        cout << "10. Find Shortest Distances and Routes\n";

        cout << "\n--- SYSTEM ALLOCATION ---\n";
        cout << "11. Allocate Resource to Highest Priority Emergency\n";

        cout << "\n0. Exit\n";

        choice = readInt("\nEnter your choice: ");

        switch (choice) {

            case 1:
                emergencyManager.addEmergency();
                break;

            case 2:
                emergencyManager.displayEmergencies();
                break;

            case 3:
                resourceManager.addResource();
                break;

            case 4:
                resourceManager.displayResources();
                break;

            case 5:
                resourceManager.releaseResource();
                break;

            case 6:
                graph.addLocation();
                break;

            case 7:
                graph.displayLocations();
                break;

            case 8:
                graph.addRoad();
                break;

            case 9:
                graph.displayRoads();
                break;

            case 10:
                graph.displayShortestDistances();
                break;

            case 11: {
                Emergency e;

                if (!emergencyManager.getNextEmergency(e)) {
                    cout << "No pending emergencies.\n";
                    break;
                }

                cout << "\nProcessing highest-priority emergency...\n";

                cout << "Victim: " << e.victimName << "\n";
                cout << "Priority: " << e.priority << "\n";

                bool success =
                    resourceManager.allocateResource(e, graph);

                if (success) {
                    emergencyManager.removeNextEmergency();

                    cout << "\nEmergency successfully processed.\n";
                }
                else {
                    cout << "\nEmergency remains in the pending queue.\n";
                }

                break;
            }

            case 0:
                cout << "\nThank you for using the system!\n";
                cout << "Program closed successfully.\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}