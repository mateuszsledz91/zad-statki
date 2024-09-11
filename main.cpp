#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <thread>

using namespace std;

const int boardSize = 30;
const int maxControlLines = 6;

class Ship {
public:
    int size;
    char symbol;

    Ship(int s, char sym) : size(s), symbol(sym) {}

    void operator--() {
        --size;
    }
};

class GameBoard {
public:
    char board[boardSize][boardSize];
    vector<Ship> ships;
    bool lastShotHit;

    GameBoard() {
        initializeBoard();
    }

    bool getLastShotHit() const {
        return lastShotHit;
    }

    void initializeBoard() {
        for (int i = 0; i < boardSize; ++i) {
            for (int j = 0; j < boardSize; ++j) {
                board[i][j] = '*';
            }
        }
    }

    void printBoard() {
        cout << setw(2) << " ";
        for (int i = 0; i < boardSize; ++i) {
            cout << setw(2) << i % 30 << " ";
        }
        cout << endl;

        for (int i = 0; i < boardSize; ++i) {
            cout << setw(2) << i % 30 << " ";
            for (int j = 0; j < boardSize; ++j) {
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
    }

    bool isValidPlacement(int row, int col, int size, bool isVertical) {
        if (isVertical) {
            if (row + size > boardSize) return false;
            for (int i = max(0, row - 1); i < min(boardSize, row + size + 1); ++i) {
                for (int j = max(0, col - 1); j <= min(boardSize - 1, col + 1); ++j) {
                    if (board[i][j] != '*') return false;
                }
            }
        } else {
            if (col + size > boardSize) return false;
            for (int i = max(0, row - 1); i <= min(boardSize - 1, row + 1); ++i) {
                for (int j = max(0, col - 1); j < min(boardSize, col + size + 1); ++j) {
                    if (board[i][j] != '*') return false;
                }
            }
        }
        return true;
    }

    void placeShip(int row, int col, int size, bool isVertical, char symbol) {
        if (isVertical) {
            for (int i = row; i < row + size; ++i) {
                board[i][col] = symbol;
            }
        } else {
            for (int j = col; j < col + size; ++j) {
                board[row][j] = symbol;
            }
        }
    }

    void manualPlacement() {
        for (Ship &ship : ships) {
            bool placedSuccessfully = false;

            do {
                printBoard();
                cout << "Place a ship of size " << ship.size << " (symbol: " << ship.symbol << "):" << endl;
                cout << "Enter starting row (0-29): ";
                int row;
                cin >> row;
                while (row < 0 || row >= boardSize) {
                    cout << "Invalid row. Enter starting row (0-29): ";
                    cin >> row;
                }

                cout << "Enter starting column (0-29): ";
                int col;
                cin >> col;
                while (col < 0 || col >= boardSize) {
                    cout << "Invalid column. Enter starting column (0-29): ";
                    cin >> col;
                }

                cout << "Enter orientation (V for vertical, H for horizontal): ";
                char orientation;
                cin >> orientation;
                orientation = toupper(orientation);
                while (orientation != 'V' && orientation != 'H') {
                    cout << "Invalid orientation. Enter V for vertical or H for horizontal: ";
                    cin >> orientation;
                }

                bool isVertical = (orientation == 'V' || orientation == 'v');

                if (isValidPlacement(row, col, ship.size, isVertical)) {
                    placeShip(row, col, ship.size, isVertical, ship.symbol);
                    placedSuccessfully = true;
                } else {
                    cout << "Invalid placement. Try again." << endl;
                    --ship;
                }
            } while (!placedSuccessfully);
        }
    }

    void autoPlacement() {
        srand(time(nullptr));

        for (Ship &ship : ships) {
            bool isVertical = rand() % 2 == 0;

            int row, col;
            do {
                row = rand() % boardSize;
                col = rand() % boardSize;
            } while (!isValidPlacement(row, col, ship.size, isVertical));

            placeShip(row, col, ship.size, isVertical, ship.symbol);
        }
    }

    bool receiveShot(int row, int col, GameBoard& opponentBoard) {
        if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
            cout << "Invalid shot coordinates. Try again." << endl;
            lastShotHit = false; // Ustaw na false, bo strzał był niepoprawny
            return false;
        }

        if (opponentBoard.isCellOccupied(row, col)) {
            cout << "Hit!" << endl;
            opponentBoard.board[row][col] = 'X';
            lastShotHit = true; // Ustaw na true, bo strzał trafił
            return true;
        } else {
            cout << "Miss." << endl;
            opponentBoard.board[row][col] = 'O';
            lastShotHit = false; // Ustaw na false, bo strzał był chybiony
            return false;
        }



    }

    bool allShipsSunk() {
        for (const Ship &ship : ships) {
            if (ship.size > 0) {
                return false;
            }
        }
        return true;
    }

private:
    bool isCellOccupied(int row, int col) {
        return (board[row][col] != '*' && board[row][col] != 'X' && board[row][col] != 'O');
    }
};

int main() {
    GameBoard playerBoard;
    GameBoard enemyBoard;

    playerBoard.ships.push_back(Ship(2, '2'));
    playerBoard.ships.push_back(Ship(3, '3'));
    playerBoard.ships.push_back(Ship(4, '4'));
    playerBoard.ships.push_back(Ship(5, '5'));

    enemyBoard.ships.push_back(Ship(2, '2'));
    enemyBoard.ships.push_back(Ship(3, '3'));
    enemyBoard.ships.push_back(Ship(4, '4'));
    enemyBoard.ships.push_back(Ship(5, '5'));

    int players;
    cout << "Choose how many players will be playing: 0-(bot v bot), 1-(player v bot), 2-(player v player): ";
    cin >> players;

    // Inicjalizacja gracza
    cout << endl << "Choose Player placement board (0-auto / 1-manual): ";
    bool playerManualPlacement;
    cin >> playerManualPlacement;
    if (playerManualPlacement) {
        playerBoard.manualPlacement();
    } else {
        playerBoard.autoPlacement();
    }

    // Inicjalizacja przeciwnika
    cout << "Choose Enemy placement board (0-auto / 1-manual): ";
    bool enemyManualPlacement;
    cin >> enemyManualPlacement;
    if (enemyManualPlacement) {
        enemyBoard.manualPlacement();
    } else {
        enemyBoard.autoPlacement();
    }

    bool playerTurn = true;
    bool hasExtraShot = false;


    while (!playerBoard.allShipsSunk() && !enemyBoard.allShipsSunk()) {
        cout << "Player's board:" << endl;
        playerBoard.printBoard();
        cout << "Enemy's board:" << endl;
        enemyBoard.printBoard();

        int shotRow, shotCol;

        if ((players == 1 && playerTurn) && !hasExtraShot) {
            cout << "Your turn:" << endl;
            cout << "Enter the row for your shot (0-29): ";
            cin >> shotRow;
            cout << "Enter the column for your shot (0-29): ";
            cin >> shotCol;

            if (playerBoard.receiveShot(shotRow, shotCol, enemyBoard) && playerBoard.getLastShotHit()) {
                hasExtraShot = true;
            }
        } else if (players == 0 && !hasExtraShot) {
            // Simulate automatic shots for both bots
            for (int botNumber = 1; botNumber <= 2; ++botNumber) {
                shotRow = rand() % boardSize;
                shotCol = rand() % boardSize;
                if (botNumber == 1) {
                    enemyBoard.receiveShot(shotRow, shotCol, playerBoard);
                } else {
                    playerBoard.receiveShot(shotRow, shotCol, enemyBoard);
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }



        }else if (players == 0) {
            // Ruch obu graczy (botów) w trybie 0 - bot vs bot
            for (int botNumber = 1; botNumber <= 2; ++botNumber) {
                cout << "Bot " << botNumber << "'s turn:" << endl;
                shotRow = rand() % boardSize;
                shotCol = rand() % boardSize;
                if (botNumber == 1) {
                    enemyBoard.receiveShot(shotRow, shotCol, playerBoard);
                } else {
                    playerBoard.receiveShot(shotRow, shotCol, enemyBoard);
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

        }else if ((players == 1 && !playerTurn) && !hasExtraShot) {
            cout << "Enemy's turn:" << endl;
            shotRow = rand() % boardSize;
            shotCol = rand() % boardSize;

            if (enemyBoard.receiveShot(shotRow, shotCol, playerBoard) && enemyBoard.getLastShotHit()) {
                hasExtraShot = true;
            }
        } else if (players == 2 && !hasExtraShot) {
            if (playerTurn) {
                cout << "Player 1's turn:" << endl;
            } else {
                cout << "Player 2's turn:" << endl;
            }

            cout << "Enter the row for your shot (0-29): ";
            cin >> shotRow;
            cout << "Enter the column for your shot (0-29): ";
            cin >> shotCol;

            if (playerTurn) {
                if (playerBoard.receiveShot(shotRow, shotCol, enemyBoard) && playerBoard.getLastShotHit()) {
                    hasExtraShot = true;
                }
            } else {
                if (enemyBoard.receiveShot(shotRow, shotCol, playerBoard) && enemyBoard.getLastShotHit()) {
                    hasExtraShot = true;
                }
            }
        }

        if (!playerBoard.allShipsSunk() && !enemyBoard.allShipsSunk()) {
            if (hasExtraShot) {
                hasExtraShot = false;
            } else {
                playerTurn = !playerTurn;
            }
        }
    }

    if (playerBoard.allShipsSunk()) {
        cout << "Game over! Player 2 wins." << endl;
    } else {
        cout << "Game over! Player 1 wins." << endl;
    }

    return 0;
}

