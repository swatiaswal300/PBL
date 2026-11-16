
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