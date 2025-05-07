#include <iostream>
#include <vector>
#include <list>
#include <unordered_set>
#include <chrono>
#include <utility>
#include <algorithm>
#include <iomanip> // ƒл€ форматированного вывода

using namespace std;
using namespace std::chrono;

const vector<string> validMoves = { "U", "D", "L", "R", "UL", "UR", "DL", "DR" };

bool isValidMove(const string& move) {
    return find(validMoves.begin(), validMoves.end(), move) != validMoves.end();
}

struct ChessField {
    int x, y;
    int visitCount;

    ChessField(int xCoord, int yCoord) : x(xCoord), y(yCoord), visitCount(1) {}

    bool operator==(const ChessField& other) const {
        return x == other.x && y == other.y;
    }

    struct Hash {
        size_t operator()(const ChessField& field) const {
            return hash<int>()(field.x) ^ hash<int>()(field.y);
        }
    };
};

struct BenchmarkResult {
    string method;
    int moves;
    double time;
    bool result;
};

bool checkWithArray(int n, const vector<string>& moves) {
    vector<ChessField> visitedFields;
    int currentX = 0, currentY = 0;
    visitedFields.emplace_back(currentX, currentY);

    for (const auto& move : moves) {
        if (move == "U") currentY++;
        else if (move == "D") currentY--;
        else if (move == "L") currentX--;
        else if (move == "R") currentX++;
        else if (move == "UL") { currentX--; currentY++; }
        else if (move == "UR") { currentX++; currentY++; }
        else if (move == "DL") { currentX--; currentY--; }
        else if (move == "DR") { currentX++; currentY--; }

        bool found = false;
        for (auto& field : visitedFields) {
            if (field.x == currentX && field.y == currentY) {
                field.visitCount++;
                if (field.visitCount >= 2) return true;
                found = true;
                break;
            }
        }

        if (!found) visitedFields.emplace_back(currentX, currentY);
    }
    return false;
}

bool checkWithLinkedList(int n, const vector<string>& moves) {
    list<ChessField> visitedFields;
    int currentX = 0, currentY = 0;
    visitedFields.emplace_back(currentX, currentY);

    for (const auto& move : moves) {
        if (move == "U") currentY++;
        else if (move == "D") currentY--;
        else if (move == "L") currentX--;
        else if (move == "R") currentX++;
        else if (move == "UL") { currentX--; currentY++; }
        else if (move == "UR") { currentX++; currentY++; }
        else if (move == "DL") { currentX--; currentY--; }
        else if (move == "DR") { currentX++; currentY--; }

        bool found = false;
        for (auto& field : visitedFields) {
            if (field.x == currentX && field.y == currentY) {
                field.visitCount++;
                if (field.visitCount >= 2) return true;
                found = true;
                break;
            }
        }

        if (!found) visitedFields.emplace_back(currentX, currentY);
    }
    return false;
}

bool checkWithSTL(int n, const vector<string>& moves) {
    unordered_set<ChessField, ChessField::Hash> visitedFields;
    int currentX = 0, currentY = 0;
    visitedFields.emplace(currentX, currentY);

    for (const auto& move : moves) {
        if (move == "U") currentY++;
        else if (move == "D") currentY--;
        else if (move == "L") currentX--;
        else if (move == "R") currentX++;
        else if (move == "UL") { currentX--; currentY++; }
        else if (move == "UR") { currentX++; currentY++; }
        else if (move == "DL") { currentX--; currentY--; }
        else if (move == "DR") { currentX++; currentY--; }

        auto it = visitedFields.find(ChessField(currentX, currentY));
        if (it != visitedFields.end()) {
            ChessField field = *it;
            field.visitCount++;
            if (field.visitCount >= 2) return true;
            visitedFields.erase(it);
            visitedFields.insert(field);
        }
        else {
            visitedFields.emplace(currentX, currentY);
        }
    }
    return false;
}

int main() {
    int n;
    cout << "Enter number of moves: ";
    cin >> n;

    vector<string> moves(n);
    cout << "Enter " << n << " moves (U, D, L, R, UL, UR, DL, DR):" << endl;
    for (int i = 0; i < n; ++i) {
        while (true) {
            cout << "Move #" << (i + 1) << ": ";
            cin >> moves[i];
            if (isValidMove(moves[i])) break;
            cout << "Invalid move! Please enter one of: U, D, L, R, UL, UR, DL, DR\n";
        }
    }

    vector<BenchmarkResult> results;

    auto testMethod = [&](const string& name, auto func) {
        auto start = high_resolution_clock::now();
        bool res = func(n, moves);
        auto stop = high_resolution_clock::now();
        double time = duration_cast<microseconds>(stop - start).count() / 1000.0;
        results.push_back({ name, n, time, res });
        };

    testMethod("Array", checkWithArray);
    testMethod("LinkedList", checkWithLinkedList);
    testMethod("STL", checkWithSTL);

    cout << "\nPerformance Comparison Summary:\n";
    cout << "==========================================================\n";
    cout << "| Method      | Moves | Time (ms) | Result               |\n";
    cout << "==========================================================\n";

    for (const auto& res : results) {
        cout << "| " << setw(11) << left << res.method << " | "
            << setw(5) << res.moves << " | "
            << setw(8) << fixed << setprecision(2) << res.time << " | "
            << setw(18) << (res.result ? "Visited twice" : "No repeats") << " |\n";
    }
    cout << "==========================================================\n";

    // –асчет и вывод относительной производительности
    if (results.size() >= 2) {
        cout << "\nRelative Performance (lower time is better):\n";
        double base_time = results[0].time;
        for (const auto& res : results) {
            cout << res.method << ": " << fixed << setprecision(2)
                << (base_time / res.time) << "x speed\n";
        }
    }

    return 0;
}