#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

class Grid {
private:
    int N;
    vector<vector<char>> grid;

public:
    Grid(int size) : N(size), grid(size, vector<char>(size, ' ')) {}

    void mark(int x, int y) {
        if (x >= 0 && x < N && y >= 0 && y < N) {
            grid[y][x] = '+';
        }
    }

    void display() const {
        for (const auto& cell : grid) {
            for (const auto& row : cell) {
                cout << (row == '+' ? '+' : ' ') << " ";
            }
            cout << endl;
        }
    }

    // Draw a straight line from (x1, y1) to (x2, y2) using Bresenham's algorithm
    void drawLine(int x1, int y1, int x2, int y2) {
        int dx = abs(x2 - x1), dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            mark(x1, y1);
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
    }
};

class Robot {
private:
    int x, y;
    Grid& grid;

public:
    Robot(Grid& g) : x(0), y(0), grid(g) {}

    void moveTo(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void lineTo(int newX, int newY) {
        grid.drawLine(x, y, newX, newY);
        moveTo(newX, newY);
    }
};

class Command {
public:
    virtual void execute(Robot& robot) = 0;
    virtual ~Command() {}
};

class MoveToCommand : public Command {
private:
    int x, y;

public:
    MoveToCommand(int x, int y) : x(x), y(y) {}

    void execute(Robot& robot) override {
        robot.moveTo(x, y);
    }
};

class LineToCommand : public Command {
private:
    int x, y;

public:
    LineToCommand(int x, int y) : x(x), y(y) {}

    void execute(Robot& robot) override {
        robot.lineTo(x, y);
    }
};

// file
class Parser {
public:
    static vector<Command*> parseCommands(const string& filename, Grid& grid) {
        vector<Command*> commands;
        ifstream file(filename);
        string line;

        if (!file.is_open()) {
            cerr << "can't open file!" << endl;
            return commands;
        }

        while (getline(file, line)) {
            stringstream ss(line);
            string command;
            ss >> command;

            if (command == "DIMENSION") {
                int N;
                ss >> N;
                if (N > 0) {
                    grid = Grid(N);  // grid z
                }
            } else if (command == "MOVE_TO") {
                int x, y;
                ss >> x >> y;
                commands.push_back(new MoveToCommand(x, y));
            } else if (command == "LINE_TO") {
                int x, y;
                ss >> x >> y;
                commands.push_back(new LineToCommand(x, y));
            } else {
                cerr << "Command error: " << command << endl;
            }
        }

        file.close();
        return commands;
    }
};

int main() {
    // Initialization
    Grid grid(5);
    Robot robot(grid);

    // get command from File
    vector<Command*> commands = Parser::parseCommands("commands.txt", grid);

    // execute
    for (Command* cmd : commands) {
        cmd->execute(robot);
    }

    // Display
    grid.display();

    // Free memory
    for (Command* cmd : commands) {
        delete cmd;
    }

    return 0;
}
