#include <queue>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <vector>
#include <stack>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include<bits/stdc++.h>
using namespace std;

const int EASY_SIZE = 15;
const int MEDIUM_SIZE = 20;
const int HARD_SIZE = 28;

char** maze;
int mazeRows, mazeCols;

int playerRow, playerCol;

enum Difficulty {
    EASY,
    MEDIUM,
    HARD
};

void generateMaze(int rows, int cols) {
    mazeRows = rows;
    mazeCols = cols;

    maze = new char*[mazeRows];
    for (int i = 0; i < mazeRows; i++) {
        maze[i] = new char[mazeCols];
        for (int j = 0; j < mazeCols; j++) {
            maze[i][j] = (rand() % 2 == 0) ? ' ' : '#';
        }
    }

    maze[mazeRows - 2][mazeCols - 2] = 'E';
    playerRow = 1;
    playerCol = 1;

    // Ensure there is a path from the player to the exit
    bool validPath = false;
    while (!validPath) {
        // Perform DFS to check for a valid path
        vector<vector<bool>> visited(mazeRows, vector<bool>(mazeCols, false));
        stack<pair<int, int>> dfsStack;
        dfsStack.push(make_pair(1, 1));
        visited[1][1] = true;

        while (!dfsStack.empty()) {
            int row = dfsStack.top().first;
            int col = dfsStack.top().second;
            dfsStack.pop();

            if (row == mazeRows - 2 && col == mazeCols - 2) {
                validPath = true;
                break;
            }

            // Explore neighboring cells
            if (row - 1 >= 0 && !visited[row - 1][col] && maze[row - 1][col] != '#') {
                dfsStack.push(make_pair(row - 1, col));
                visited[row - 1][col] = true;
            }
            if (row + 1 < mazeRows && !visited[row + 1][col] && maze[row + 1][col] != '#') {
                dfsStack.push(make_pair(row + 1, col));
                visited[row + 1][col] = true;
            }
            if (col - 1 >= 0 && !visited[row][col - 1] && maze[row][col - 1] != '#') {
                dfsStack.push(make_pair(row, col - 1));
                visited[row][col - 1] = true;
            }
            if (col + 1 < mazeCols && !visited[row][col + 1] && maze[row][col + 1] != '#') {
                dfsStack.push(make_pair(row, col + 1));
                visited[row][col + 1] = true;
            }
        }

        if (!validPath) {
            // Regenerate the maze
            for (int i = 0; i < mazeRows; i++) {
                delete[] maze[i];
            }
            delete[] maze;
            maze = new char*[mazeRows];
            for (int i = 0; i < mazeRows; i++) {
                maze[i] = new char[mazeCols];
                for (int j = 0; j < mazeCols; j++) {
                    maze[i][j] = (rand() % 2 == 0) ? ' ' : '#';
                }
            }
            maze[mazeRows - 2][mazeCols - 2] = 'E';
        }
    }
}

void displayMaze() {
    const int PATH_COLOR = 13;

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

    system("cls");
    for (int i = 0; i < mazeRows; i++) {
        for (int j = 0; j < mazeCols; j++) {
#ifdef _WIN32
            if (i == playerRow && j == playerCol) {
                SetConsoleTextAttribute(hConsole, 10);  // Set text color to green
                cout << 'P';
            } else if (maze[i][j] == 'E') {
                SetConsoleTextAttribute(hConsole, 12);  // Set text color to red
                cout << 'E';
            } else if (maze[i][j] == '.') {
                SetConsoleTextAttribute(hConsole, PATH_COLOR);  // Set text color to blue
                cout << '.';
            } else {
                SetConsoleTextAttribute(hConsole, 7);   // Set text color to white
                cout << maze[i][j];
            }
#else
            if (i == playerRow && j == playerCol) {
                cout << "\033[1;32mP\033[0m";  // Set text color to green
            } else if (maze[i][j] == 'E') {
                cout << "\033[1;31mE\033[0m";  // Set text color to red
            } else if (maze[i][j] == '.') {
                cout << "\033[1;" << BLUE_COLOR << "m.\033[0m";  // Set text color to blue
            } else {
                cout << maze[i][j];
            }
#endif
        }
        cout << endl;
    }
}

bool isMoveValid(int row, int col) {
    if (row >= 0 && row < mazeRows && col >= 0 && col < mazeCols && maze[row][col] != '#') {
        return true;
    }
    return false;
}

void movePlayer(char direction) {
    int newRow = playerRow;
    int newCol = playerCol;

    switch (direction) {
        case 'W':
        case 'w':
            newRow--;
            break;
        case 'S':
        case 's':
            newRow++;
            break;
        case 'A':
        case 'a':
            newCol--;
            break;
        case 'D':
        case 'd':
            newCol++;
            break;
        default:
            return;
    }

    if (isMoveValid(newRow, newCol)) {
        playerRow = newRow;
        playerCol = newCol;
        displayMaze();
    }
}

bool hasReachedExit() {
    return maze[playerRow][playerCol] == 'E';
}

void cleanUp() {
    for (int i = 0; i < mazeRows; i++) {
        delete[] maze[i];
    }
    delete[] maze;
}

void autoMoveToExit() {
    if (playerRow == mazeRows - 2 && playerCol == mazeCols - 2) {
        // The player is already at the exit, no need to move
        return;
    }

    vector<vector<bool>> visited(mazeRows, vector<bool>(mazeCols, false));
    vector<vector<pair<int, int>>> parent(mazeRows, vector<pair<int, int>>(mazeCols));
    queue<pair<int, int>> bfsQueue;
    bfsQueue.push(make_pair(playerRow, playerCol));
    visited[playerRow][playerCol] = true;

    while (!bfsQueue.empty()) {
        int row = bfsQueue.front().first;
        int col = bfsQueue.front().second;
        bfsQueue.pop();

        if (row == mazeRows - 2 && col == mazeCols - 2) {
            // Found the exit, reconstruct the path and mark it with '.'
            while (!(row == playerRow && col == playerCol)) {
                int prevRow = parent[row][col].first;
                int prevCol = parent[row][col].second;
                maze[prevRow][prevCol] = '.';
                row = prevRow;
                col = prevCol;
            }
            break;
        }

        // Explore neighboring cells
        if (row - 1 >= 0 && !visited[row - 1][col] && maze[row - 1][col] != '#') {
            bfsQueue.push(make_pair(row - 1, col));
            visited[row - 1][col] = true;
            parent[row - 1][col] = make_pair(row, col);
        }
        if (row + 1 < mazeRows && !visited[row + 1][col] && maze[row + 1][col] != '#') {
            bfsQueue.push(make_pair(row + 1, col));
            visited[row + 1][col] = true;
            parent[row + 1][col] = make_pair(row, col);
        }
        if (col - 1 >= 0 && !visited[row][col - 1] && maze[row][col - 1] != '#') {
            bfsQueue.push(make_pair(row, col - 1));
            visited[row][col - 1] = true;
            parent[row][col - 1] = make_pair(row, col);
        }
        if (col + 1 < mazeCols && !visited[row][col + 1] && maze[row][col + 1] != '#') {
            bfsQueue.push(make_pair(row, col + 1));
            visited[row][col + 1] = true;
            parent[row][col + 1] = make_pair(row, col);
        }
    }

    // Follow the marked path step by step
    vector<pair<int, int>> path;
    int currentRow = mazeRows - 2;
    int currentCol = mazeCols - 2;

    while (currentRow != playerRow || currentCol != playerCol) {
        path.push_back(make_pair(currentRow, currentCol));
        int newRow = parent[currentRow][currentCol].first;
        int newCol = parent[currentRow][currentCol].second;
        currentRow = newRow;
        currentCol = newCol;
    }

    // Move the player along the path step by step
    reverse(path.begin(), path.end());

    for (const auto& step : path) {
        playerRow = step.first;
        playerCol = step.second;
        displayMaze();

        // Delay between each step for better visualization
        // You can adjust the duration as desired (in milliseconds)
        // For example, sleep(1000) would introduce a 1-second delay between steps.
#ifdef _WIN32
        Sleep(500);
#else
        usleep(500000);
#endif
    }
}


int main() {
    srand(time(0));

    char move;
    Difficulty difficulty;

    cout << "Welcome to the Maze Game!" << endl;
    cout << "Choose a difficulty level: " << endl;
    cout << "1. Easy" << endl;
    cout << "2. Medium" << endl;
    cout << "3. Hard" << endl;
    cout << "Enter your choice (1-3): ";
    int choice;
    cin >> choice;

    switch (choice) {
        case 1:
            difficulty = EASY;
            generateMaze(EASY_SIZE, EASY_SIZE);
            break;
        case 2:
            difficulty = MEDIUM;
            generateMaze(MEDIUM_SIZE, MEDIUM_SIZE);
            break;
        case 3:
            difficulty = HARD;
            generateMaze(HARD_SIZE, HARD_SIZE);
            break;
        default:
            cout << "Invalid choice. Exiting the game." << endl;
            return 0;
    }

    displayMaze();

    cout << "Instructions:" << endl;
    cout << "Use 'W', 'A', 'S', 'D' to move the player." << endl;
    cout << "Press 'G' to automatically move the player step by step to the exit." << endl;
    cout << "Press 'Q' to quit the game." << endl;

    while (true) {
        if (_kbhit()) {
            move = _getch();
            if (move == 'Q' || move == 'q') {
                break;
            } else if (move == 'G' || move == 'g') {
                autoMoveToExit();
            } else {
                movePlayer(move);
            }

            if (hasReachedExit()) {
                cout << "Congratulations! You reached the exit." << endl;
                break;
            }
        }
    }

    cleanUp();

    return 0;
}