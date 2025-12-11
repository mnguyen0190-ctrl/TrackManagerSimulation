#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <string>
#include <limits>
#include <algorithm>
#include <list>
using namespace std;

/**
 * @author : Matthew Nguyen
 * @created : 12/8/25
**/

struct Lap {
    int lapNumber;
    double lapTime;
};

struct Driver {
    int id;
    string name;
    int carNumber;

    stack<Lap> lapHistory;
};

struct driverStats {
    int totalLaps = 0;
    double totalTime = 0.0;
    int pitStops = 0;
};

struct Edge {
    int next;
    double length;
};

class TrackGraph {
private:
    vector<vector<Edge>> list;
public:
    TrackGraph(int numTurns) {
        list.resize(numTurns);
    }
    void addTurn() {
        list.push_back({});
    }

    int turnCount() const {
        return (int)list.size();
    }

    void addSegment(int prev, int next, double length) {
        if (prev < 0 || prev >= (int)list.size() || next < 0 || next >= (int)list.size()) {
            cout << "Invalid segment. \n";
            return;
        }
        list[prev].push_back({next, length});
    }

    void removeSegment(int prev, int next) {
        if (prev < 0 || prev >= (int)list.size()) return;
        auto &segment = list[prev];

        for (auto it = segment.begin(); it != segment.end(); ) {
            if (it->next == next) {
                it = segment.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    double getSegmentLength(int prev, int next, bool &found) const {
        found = false;
        if (prev < 0 || prev > (int)list.size()) return 0.0;
        for (const auto &e : list[prev]) {
            if (e.next == next) {
                found = true;
                return e.length;
            }
        }
        return 0.0;
    }

    void clearAll() {
        list.clear();
        cout << "Track cleared, no turns remain.\n";
    }

    double computeLapDistance() const {
        double total = 0.0;
        for (int i = 0; i < list.size(); i++) {
            for (const auto &e : list[i]) {
                total += e.length;
            }
        }
        return total;
    }

    void display() const {
        cout << "Track Layout: \n";
        if (list.empty()) {
            cout << "(no turns inputted)\n";
            return;
        }
        for (int i = 0; i < list.size(); ++i) {
            cout << " Turn " << (i + 1) << " -> ";
            for (const auto &e : list[i]) {
                cout << "(Turn " << (e.next + 1) << ", " << e.length << "m) ";
            }
            cout << "\n";
        }
    }
};

struct BracketNode {
    int driverId;
    BracketNode *left, *right;
    BracketNode(int id = -1) : driverId(id), left(nullptr), right(nullptr) {}
};

class Tournament {
private:
    BracketNode *root = nullptr;

    void clear(BracketNode *node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    BracketNode* buildBracket(vector<BracketNode*> &leaves) {
        if (leaves.empty()) return nullptr;
        if (leaves.size() == 1) return leaves[0];

        vector<BracketNode*> parents;
        parents.reserve(leaves.size() / 2);

        for (size_t i = 0; i < leaves.size(); i += 2) {
            BracketNode *left  = leaves[i];
            BracketNode *right = leaves[i + 1];

            BracketNode *parent = new BracketNode(-1);
            parent->left = left;
            parent->right = right;
            parents.push_back(parent);
        }
        return buildBracket(parents);
    }


    void printBracket(BracketNode *node, int depth, const unordered_map<int, string> &idToCar) const {
        if (!node) return;
        printBracket(node->right, depth + 1, idToCar);
        for (int i = 0; i < depth; i++) cout << "       ";
        if (node->driverId == -1) cout << "[TBD]\n";
        else {
            auto left = idToCar.find(node->driverId);
            if (left != idToCar.end()) {
                cout << left->second << "\n";
            } else {
                cout << "Driver " << node->driverId << '\n';
            }
        }
        printBracket(node->left, depth + 1, idToCar);
    }

    void collectMatches(BracketNode *node, vector<BracketNode*> &matches) const {
        if (!node) return;

        if (node->left || node->right) {
            matches.push_back(node);
        }
        collectMatches(node->left, matches);
        collectMatches(node->right, matches);
    }
public:
    Tournament() = default;

    ~Tournament() {
        clear(root);
    }

    bool hasBracket() const {
        return root != nullptr;
    }

    void build(const vector<int> &ids) {
        clear(root);
        root = nullptr;

        if (ids.empty()) {
            cout << "No drivers available for bracket.\n";
            return;
        }

        int n = (int)ids.size();

        int leafCount = 1;
        while (leafCount * 2 <= n) {
            leafCount *= 2;
        }

        if (leafCount < n) {
            cout << "Bracket can only include " << leafCount
                 << " drivers for matching."
                 << "Drivers by ID will not be in the bracket.\n";
            for (int i = leafCount; i < n; ++i) {
                cout << "  Driver ID " << ids[i] << '\n';
            }
        }

        vector<BracketNode*> leaves;
        leaves.reserve(leafCount);

        for (int i = 0; i < leafCount; ++i) {
            leaves.push_back(new BracketNode(ids[i]));
        }
        root = buildBracket(leaves);
    }

    void display(const unordered_map<int,string> &nameMap) const {
        cout << "Tournament Bracket (Tree):\n";
        if (!root) {
            cout << " (no bracket built yet)\n";
            return;
        }
        printBracket(root, 0, nameMap);
    }

    int listMatches(const unordered_map<int,string> &nameMap) const {
        if (!root) {
            cout << " (no bracket built yet)\n";
            return 0;
        }

        vector<BracketNode*> matches;
        collectMatches(root, matches);

        int idx = 1;
        for (auto *m : matches) {
            if (!m->left || !m->right) continue; 
            int leftId = m->left->driverId;
            int rightId = m->right->driverId;

            cout << "Match " << idx << ":\n";

            cout << "  1. ";
            if (leftId == -1) cout << "[TBD]\n";
            else {
                auto it = nameMap.find(leftId);
                if (it != nameMap.end()) cout << it->second << "\n";
                else cout << "Driver " << leftId << "\n";
            }

            cout << "  2. ";
            if (rightId == -1) cout << "[TBD]\n";
            else {
                auto it = nameMap.find(rightId);
                if (it != nameMap.end()) cout << it->second << "\n";
                else cout << "Driver " << rightId << "\n";
            }
            idx++;
        }
        return idx - 1;
    }

    
    bool setWinnerByMatchIndex(int matchIndex, int winnerSide) {
        if (!root) return false;

        vector<BracketNode*> matches;
        collectMatches(root, matches);

        if (matchIndex < 1 || matchIndex > (int)matches.size()) return false;
        BracketNode *match = matches[matchIndex - 1];

        if (!match->left || !match->right) return false;

        int chosenId = -1;
        if (winnerSide == 1) chosenId = match->left->driverId;
        else if (winnerSide == 2) chosenId = match->right->driverId;
        else return false;

        if (chosenId == -1) return false; 

        match->driverId = chosenId;
        return true;
    }
};



int nextDriverId = 1;

class DriverEdit {
private:
    list<Driver> &drivers;
    unordered_map<int, driverStats> &stats;
public:
    DriverEdit(list<Driver> &d, unordered_map<int, driverStats> &s)
        : drivers(d), stats(s) {}

    void menu() {
        int choice = -11;

        while (choice != 0) {
            cout << "\n=== Driver Edit Menu ===\n"
                 << "1. Add Driver\n"
                 << "2. Edit Driver\n"
                 << "3. List Drivers\n"
                 << "4. Remove Driver\n"      
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;

            if (choice == 1) addDriver();
            else if (choice == 2) editDriver();
            else if (choice == 3) listDrivers();
            else if (choice == 4) removeDriver();   
        }
    }

private:
    void addDriver() {
        string name;
        int car;

        cout << "Name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Car Number: ";
        cin >> car;

        Driver d;
        d.id = nextDriverId++;
        d.name = name;
        d.carNumber = car;

        drivers.push_back(d);
        stats[d.id] = driverStats();

        cout << "Driver added.\n";
    }

    void editDriver() {
        if (drivers.empty()) {
            cout << "No drivers to edit.\n";
            return;
        }

        cout << "\nSelect driver to edit:\n";
        int index = 1;
        vector<Driver*> driverList;
        for (auto &d : drivers) {
            cout << index << ". " << d.name << " | Car " << d.carNumber << '\n';
            driverList.push_back(&d);
            index++;
        }

        int choice;
        cout << "Enter number: ";
        cin >> choice;

        if (!cin || choice < 1 || choice > (int)driverList.size()) {
            cout << "Invalid selection.\n";
            return;
        }

        Driver *d = driverList[choice - 1];

        string newName;
        int newCar;

        cout << "New name (blank to keep): ";
        cin.ignore();
        getline(cin, newName);
        if (!newName.empty()) d->name = newName;

        cout << "New car number (-1 to keep): ";
        cin >> newCar;
        if (newCar != -1) d->carNumber = newCar;

        cout << "Driver updated.\n";
    }

    void removeDriver() {
        if (drivers.empty()) {
            cout << "No drivers to remove.  q\n";
            return;
        }

        
        cout << "\nSelect driver to remove:\n";
        int index = 1;
        vector<Driver*> driverList;
        for (auto &d : drivers) {
            cout << index << ". " << d.name << " | Car " << d.carNumber << '\n';
            driverList.push_back(&d);
            index++;
        }

        int choice;
        cout << "Enter number: ";
        cin >> choice;

        if (choice < 1 || choice > (int)driverList.size()) {
            cout << "Invalid selection.\n";
            return;
        }

        Driver *target = driverList[choice - 1];
        int removedId  = target->id;

        stats.erase(removedId);

        for (auto item = drivers.begin(); item != drivers.end(); ++item) {
            if (&(*item) == target) {
                cout << "Removing driver: " << item->name << " | Car " << item->carNumber << '\n';
                drivers.erase(item);
                break;
            }
        }

        cout << "Driver removed.\n";
        cout << "Note: If you are using the tournament bracket, "
             << "rebuild it so it no longer includes this driver.\n";
    }


    void listDrivers() {
        cout << "Driver List:\n";
        for (auto &d : drivers) {
            cout << d.name << " | Car " << d.carNumber << "\n";
        }
    }
};


class TrackEdit {
private:
    TrackGraph &track;
public:
    TrackEdit(TrackGraph &g) : track(g) {}

    void menu() {
        int choice = -1;

        while (choice != 0) {
            cout << "\n=== Track Edit Menu ===\n"
                 << "1. Add Turn (LAST TURN -> TURN 1 is fixed at 500m for a fixed straight)\n"
                 << "2. Clear Track\n"
                 << "3. Show Track Layout\n"
                 << "4. Show Turn Count & Lap Distance\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;

            if  (choice == 1) addTurn();
            else if (choice == 2) clearTrack();
            else if (choice == 3) track.display();
            else if (choice == 4) showTrackInfo();
        }
    }

private:
    void addTurn() {
        int turnCount = track.turnCount();

        
        if (turnCount == 0) {
            track.addTurn();
            cout << "First turn added. Total turns: 1\n";
            return;
        }

        bool hasLastTurn = false;
        double lastLength = 0.0;
        int indexLastTurn = -1;
        if (turnCount >= 2) {
            indexLastTurn = turnCount - 1;
            lastLength = track.getSegmentLength(indexLastTurn, 0, hasLastTurn);
        }
            track.addTurn();
            int totalTurns = track.turnCount();
            int newIndex = totalTurns - 1;
            int newTurnNumber = totalTurns;
            int prevIndex = newIndex - 1;
            int prevTurnNumber = newTurnNumber - 1;


            double length;
            cout << "Enter distance (m) from Turn " << prevTurnNumber << " to Turn " << newTurnNumber << ": ";
            cin >> length;

            if (length <= 0) {
                cout << "Invalid distance. Turn added without segment.\n";
                cout << "Total turns: " << track.turnCount() << "\n";
                return;
            }

            track.addSegment(prevIndex, newIndex, length);

            if (hasLastTurn) {
                track.removeSegment(indexLastTurn, 0);
                track.addSegment(newIndex, 0, lastLength);
            } else {
                if (track.turnCount() >= 2) {
                    track.addSegment(newIndex, 0, 500);
                }
            }

            cout << "New turn added. Total turns: " << track.turnCount() << "\n";
            cout << "Segment created: Turn " << prevTurnNumber << " -> Turn " << newTurnNumber << " (" << length << "m)\n";
    }

    void clearTrack() {
        track.clearAll();
    }

    void showTrackInfo() {
        cout << "Total turns: " << track.turnCount() << '\n';
        cout << "Approx lap distance: " << track.computeLapDistance() << " m\n";
    }
};

class BracketEdit {
private:
    Tournament &bracket;
    list<Driver> &drivers;

public:
    BracketEdit(Tournament &b, list<Driver> &d) : bracket(b), drivers(d) {}

    void menu() {
        int choice = -1;

        while (choice != 0) {
            cout << "\n=== Bracket Edit Menu ===\n"
                 << "1. Rebuild Bracket (Only the first 4/8/16 drivers will be used.)\n"
                 << "2. Show Bracket\n"
                 << "3. Set Match Winner\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;
            
            if (choice == 1) rebuild();
            else if (choice == 2) showBracket();
            else if (choice == 3) setWinnerMenu();
        }
    }

private:
    void rebuild() { vector<int> ids;
        for (auto &d : drivers) {
            ids.push_back(d.id);
        }
        bracket.build(ids);
        cout << "Bracket rebuilt for " << ids.size() << " driver(s).\n";
    }


    void showBracket() {
        unordered_map<int,string> nameMap;
        for (auto &d : drivers) {
            nameMap[d.id] = d.name;
        }
        bracket.display(nameMap);
    }

    void setWinnerMenu() {
        if (!bracket.hasBracket()) {
            cout << "No bracket built yet.\n";
            cout << "Use 'Rebuild Bracket' first.\n";
            return;
        }

        unordered_map<int,string> nameMap;
        for (auto &d : drivers) {
            nameMap[d.id] = d.name;
        }

        cout << "\nCurrent Matches:\n";
        int total = bracket.listMatches(nameMap);
        if (total == 0) {
            cout << "No matches available.\n";
            return;
        }

        cout << "Select match number (1-" << total << "): ";
        int matchIndex;
        cin >> matchIndex;

        if (matchIndex < 1 || matchIndex > total) {
            cout << "Invalid match.\n";
            return;
        }

        cout << "Choose winner (1 = left, 2 = right): ";
        int winnerSide;
        cin >> winnerSide;

        if (!bracket.setWinnerByMatchIndex(matchIndex, winnerSide)) {
            cout << "Failed to set winner (Selected TBD or invalid input).\n";
        } else {
            cout << "Winner advanced.\n";
        }
    }
};

class RaceManager {
    
    list<Driver> drivers;
    unordered_map<int, driverStats> stats;
    
    queue<int> pitQueue;

    TrackGraph track;
    Tournament bracket;

    DriverEdit driverMenu;
    TrackEdit trackMenu;
    BracketEdit bracketMenu;

public:
    RaceManager()
        : track(4),
          driverMenu(drivers, stats),
          trackMenu(track),
          bracketMenu(bracket, drivers) {

        track.addSegment(0, 1, 300.0);
        track.addSegment(1, 2, 150.0);
        track.addSegment(2, 3, 25.0);
        track.addSegment(3, 0, 500.0);
    }

    void addDriver(int id, const string &name, int carNumber) {
        Driver driver;
        driver.id = id;
        driver.name = name;
        driver.carNumber = carNumber;
        drivers.push_back(driver);
        stats[id] = driverStats();
    }

    void showDrivers() const {
        cout << "Drivers (in current order):\n";
        for (const auto &d : drivers) {
            cout << "Name: " << d.name << " | Car: " << d.carNumber << "\n";
        }
    }

    Driver* selectDriverFromList() {
        if (drivers.empty()) {
            cout << "No drivers available.\n";
            return nullptr;
        }
        cout << "\nSelect a driver:\n";
        int index = 1;
        vector<Driver*> driverList;
        for (auto &d : drivers) {
            cout << index << ". " << d.name << " | Car " << d.carNumber << '\n';
            driverList.push_back(&d);
            index++;
        }
        int choice;
        cout << "Enter number: ";
        cin >> choice;

        if (!cin || choice < 1 || choice > (int)driverList.size()) {
            cout << "Invalid selection.\n";
            return nullptr;
        }
        return driverList[choice - 1];
    }

    void recordLap(int driverId, double lapTime) {
        for (auto &d : drivers) {
            if (d.id == driverId) {
                Lap lap;
                lap.lapNumber = (int)d.lapHistory.size() + 1;
                lap.lapTime = lapTime;
                d.lapHistory.push(lap);

                driverStats &st = stats[d.id];
                st.totalLaps++;
                st.totalTime += lapTime;

                cout << "Recorded lap " << lap.lapNumber
                     << " for " << d.name
                     << " in " << lapTime << " seconds.\n";
                return;
            }
        }
        cout << "Driver not found.\n";
    }

    void showLapHistory(int driverId) const {
        for (const auto &d : drivers) {
            if (d.id == driverId) {
                cout << "Lap history for " << d.name << ":\n";
                stack<Lap> copy = d.lapHistory;
                if (copy.empty()) {
                    cout << " (no laps yet)\n";
                    return;
                }
                while (!copy.empty()) {
                    Lap lap = copy.top();
                    copy.pop();
                    cout << " Lap " << lap.lapNumber << ": " << lap.lapTime << " s\n";
                }
                return;
            }
        }
        cout << "Driver not found.\n";
    }

    void queuePitstop(int driverId) {
        for (const auto &d : drivers) {
            if (d.id == driverId) {
                pitQueue.push(d.carNumber);
                cout << "Car " << d.carNumber << " (" << d.name
                     << ") has joined pit queue.\n";
                return;
            }
        }
        cout << "Driver not found.\n";
    }

    void processPitstop() {
        if (pitQueue.empty()) {
            cout << "Nobody in queue.\n";
            return;
        }
        int carNum = pitQueue.front();
        pitQueue.pop();

        for (const auto &d : drivers) {
            if (d.carNumber == carNum) {
                stats[d.id].pitStops++;
                cout << "Car " << carNum << " (" << d.name
                     << ") is exiting pit stop.\n";
                return;
            }
        }
    }

    void showPitQueue() const {
        cout << "Pit Queue:\n";
        if (pitQueue.empty()) {
            cout << "   (empty)\n";
            return;
        }
        queue<int> copy = pitQueue;
        while (!copy.empty()) {
            int carNum = copy.front();
            copy.pop();
            cout << "Car " << carNum << '\n';
        }
    }

    void showTrackinfo() const {
        track.display();
        double dist = track.computeLapDistance();
        cout << "Lap distance: " << dist << "m\n";
    }

    
    void buildAndShowTournament() {
        unordered_map<int,string> nameMap;
        for (auto &d : drivers) {
            nameMap[d.id] = d.name;
        }

        if (!bracket.hasBracket()) {
            cout << "No bracket built yet.\n";
            cout << "Use 'Bracket Edit Menu -> Rebuild Bracket' to create one from current drivers.\n";
            return;
        }

        bracket.display(nameMap);
    }

    void runMenu() {
        int choice;

        do {
            cout << "\n=== Track Manager Simulation Menu ===\n"
                 << "1. Show drivers\n"
                 << "2. Record lap\n"
                 << "3. Show lap history for driver\n"
                 << "4. Request pit stop\n"
                 << "5. Process next pit stop\n"
                 << "6. Show pit queue\n"
                 << "7. Show track information\n"
                 << "8. Show tournament bracket\n"
                 << "9. Driver Edit Menu\n"
                 << "10. Track Edit Menu\n"
                 << "11. Bracket Edit Menu\n"
                 << "0. Exit\n"
                 << "Enter choice: ";

            cin >> choice;

            if (!cin) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                choice = -1;
            }

            switch (choice) {
                case 1:
                    showDrivers();
                    break;

                case 2: {
                    Driver *d = selectDriverFromList();
                    if (!d) break;
                    double time;
                    cout << "Lap time (s): ";
                    cin >> time;
                    recordLap(d->id, time);
                    break;
                }

                case 3: {
                    Driver *d = selectDriverFromList();
                    if (!d) break;
                    showLapHistory(d->id);
                    break;
                }

                case 4: {
                    Driver *d = selectDriverFromList();
                    if (!d) break;
                    queuePitstop(d->id);
                    break;
                }

                case 5:
                    processPitstop();
                    break;

                case 6:
                    showPitQueue();
                    break;

                case 7:
                    showTrackinfo();
                    break;

                case 8:
                    buildAndShowTournament();
                    break;

                case 9:
                    driverMenu.menu();
                    break;

                case 10:
                    trackMenu.menu();
                    break;

                case 11:
                    bracketMenu.menu();
                    break;

                case 0:
                    cout << "Exiting...\n";
                    break;

                default:
                    cout << "Invalid choice. Try again.\n";
            }

        } while (choice != 0);
    }
};

int main() {
    RaceManager manager;

    manager.addDriver(1, "Alice",   11);
    manager.addDriver(2, "Bob",     22);
    manager.addDriver(3, "Charlie", 33);
    manager.addDriver(4, "Diana",   44);
    manager.addDriver(5, "Eve",     55);
    manager.addDriver(6, "Frank",   66);

    manager.runMenu();
    return 0;
}
