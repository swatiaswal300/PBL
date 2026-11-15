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