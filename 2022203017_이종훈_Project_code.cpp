#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <ncurses.h>
#include <stack>
#include <string>
#include <unistd.h>

using namespace std;
WINDOW *win1;
WINDOW *win2;
WINDOW *win3;
WINDOW *win4;
WINDOW *win5;
WINDOW *win6;
WINDOW *win7;
WINDOW *win8;

bool mainBool = true;
bool gameBool = true;
bool chkGameBool = true;

void loginPage();
void registerPage();

const char *userFile = "users.txt";
bool authenticateUser(const char *username, const char *password);
bool addUser(const char *username, const char *password);
bool isUserExists(int fileDescriptor, const char *username);

void gameSelectPage();
void selectGame();
void wordle();
void afterGame();
void snake();

struct User {
    string username;
    string password;
};

const int BOARD_SIZE = 20;

class SnakeGame {
  private:
    deque<pair<int, int>> snake;
    pair<int, int> food;
    int direction;
    bool gameOver;

  public:
    SnakeGame() {
        initializeGame();
        initializeBoard();
    }

    void play() {
        while (!gameOver) {
            displayBoard();
            processInput();
            moveSnake();
            checkCollision();
            checkFood();
            usleep(100000);
        }

        clear();
        refresh();
        printw("Game Over! Your Score : %d \n", snake.size() - 1);
        printw("If you want to go back to game select screen, press f\n");
        printw("or if you want to end this program, press ESC\n");
        afterGame();
    }

  private:
    void initializeGame() {
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        curs_set(0);

        snake.push_front({BOARD_SIZE / 2, BOARD_SIZE / 2});
        direction = 1;
        spawnFood();

        gameOver = false;

        initializeBoard();
        refresh();
    }

    void initializeBoard() {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            mvprintw(i, 0, "#");
            mvprintw(i, BOARD_SIZE - 1, "#");
        }

        for (int j = 0; j < BOARD_SIZE; ++j) {
            mvprintw(0, j, "#");
            mvprintw(BOARD_SIZE - 1, j, "#");
        }
    }

    void displayBoard() {
        clear();

        for (auto segment : snake) {
            mvprintw(segment.first + 1, segment.second + 1, "O");
        }

        mvprintw(food.first + 1, food.second + 1, "X");

        for (int i = 0; i < BOARD_SIZE + 2; ++i) {
            mvprintw(0, i, "#");
            mvprintw(BOARD_SIZE + 1, i, "#");
        }

        for (int j = 0; j < BOARD_SIZE + 2; ++j) {
            mvprintw(j, 0, "#");
            mvprintw(j, BOARD_SIZE + 1, "#");
        }

        refresh();
    }

    void processInput() {
        int ch = getch();

        switch (ch) {
        case KEY_UP:
            if (direction != 2)
                direction = 0;
            break;
        case KEY_RIGHT:
            if (direction != 3)
                direction = 1;
            break;
        case KEY_DOWN:
            if (direction != 0)
                direction = 2;
            break;
        case KEY_LEFT:
            if (direction != 1)
                direction = 3;
            break;
        case 'q':
            gameOver = true;
            break;
        }
    }

    void moveSnake() {
        int newHeadRow = snake.front().first;
        int newHeadCol = snake.front().second;

        switch (direction) {
        case 0: // up
            newHeadRow--;
            break;
        case 1: // right
            newHeadCol++;
            break;
        case 2: // down
            newHeadRow++;
            break;
        case 3: // left
            newHeadCol--;
            break;
        }

        snake.push_front({newHeadRow, newHeadCol});

        mvprintw(snake.back().first, snake.back().second, " ");
        snake.pop_back();
    }

    void checkCollision() {

        if (snake.front().first == 0 || snake.front().first == BOARD_SIZE - 1 ||
            snake.front().second == 0 ||
            snake.front().second == BOARD_SIZE - 1) {
            gameOver = true;
        }

        for (auto it = next(snake.begin()); it != snake.end(); ++it) {
            if (snake.front() == *it) {
                gameOver = true;
                break;
            }
        }
    }

    void spawnFood() {
        srand(time(0));
        int row, col;

        do {
            row = rand() % (BOARD_SIZE - 2) + 1;
            col = rand() % (BOARD_SIZE - 2) + 1;
        } while (isSnakeSegment(row, col));

        food = {row, col};
    }

    void checkFood() {
        if (snake.front() == food) {
            snake.push_back({-1, -1});
            spawnFood();
        }
    }

    bool isSnakeSegment(int row, int col) {
        for (auto segment : snake) {
            if (segment.first == row && segment.second == col) {
                return true;
            }
        }
        return false;
    }
};

void mainPage() {
    WINDOW *win1 = newwin(15, 80, 0, 0);
    WINDOW *win2 = newwin(9, 30, 15, 0);
    WINDOW *win3 = newwin(9, 30, 15, 30);
    WINDOW *win4 = newwin(9, 30, 15, 60);

    wbkgd(win1, COLOR_PAIR(1));
    wbkgd(win2, COLOR_PAIR(3));
    wbkgd(win3, COLOR_PAIR(4));
    wbkgd(win4, COLOR_PAIR(3));

    mvwprintw(win1, 5, 30, "JongHoon's GAMEPACK");
    mvwprintw(win2, 5, 10, "LOGIN");
    mvwprintw(win3, 5, 10, "REGISTER");
    mvwprintw(win4, 5, 10, "EXIT");
    mvwprintw(win2, 2, 10, "PRESS 1");
    mvwprintw(win3, 2, 10, "PRESS 2");
    mvwprintw(win4, 2, 10, "PRESS ESC");

    wrefresh(win1);
    wrefresh(win2);
    wrefresh(win3);
    wrefresh(win4);
}

void loginPage() {
    clear();

    WINDOW *loginWin = newwin(8, 40, 10, 20);
    wbkgd(loginWin, COLOR_PAIR(3));
    box(loginWin, 0, 0);

    mvwprintw(loginWin, 2, 15, "LOGIN");

    mvwprintw(loginWin, 4, 5, "Username: ");
    wrefresh(loginWin);

    char username[20];
    echo();
    wgetnstr(loginWin, username, 20);
    noecho();

    mvwprintw(loginWin, 6, 5, "Password: ");
    wrefresh(loginWin);

    char password[20];
    echo();
    wgetnstr(loginWin, password, 20);
    noecho();

    if (authenticateUser(username, password)) {
        clear();
        printw("Login successful!\n");
        refresh();
        getch();
        delwin(loginWin);
        mainBool = false;
        gameSelectPage();

    } else {
        clear();
        printw("Login failed. Invalid username or password.\n");
        refresh();
        getch();
        delwin(loginWin);
        mainPage();
    }
}

void registerPage() {
    clear();

    WINDOW *registerWin = newwin(8, 40, 10, 20);
    wbkgd(registerWin, COLOR_PAIR(4));
    box(registerWin, 0, 0);

    mvwprintw(registerWin, 2, 15, "REGISTER");

    mvwprintw(registerWin, 4, 5, "Username: ");
    wrefresh(registerWin);

    char username[20];
    echo();
    wgetnstr(registerWin, username, 20);
    noecho();

    mvwprintw(registerWin, 6, 5, "Password: ");
    wrefresh(registerWin);

    char password[20];
    echo();
    wgetnstr(registerWin, password, 20);
    noecho();

    if (addUser(username, password)) {
        clear();
        printw("Registration successful!\n");
        refresh();
        getch();
    } else {
        clear();
        printw("Registration failed. Username already exists.\n");
        refresh();
        getch();
    }

    delwin(registerWin);
    mainPage();
}

bool isUserExists(int fileDescriptor, const char *username) {
    char buffer[256];
    ssize_t bytesRead;

    lseek(fileDescriptor, 0, SEEK_SET);

    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        char *line = strtok(buffer, "\n");

        while (line != nullptr) {
            char *colonPos = strchr(line, ':');

            if (colonPos != nullptr) {
                *colonPos = '\0';

                if (strcmp(line, username) == 0) {
                    return true;
                }
            }

            line = strtok(nullptr, "\n");
        }
    }

    return false;
}

bool authenticateUser(const char *username, const char *password) {
    const char *userFile = "users.txt";
    int fileDescriptor = open(userFile, O_RDONLY);

    if (fileDescriptor == -1) {
        cerr << "Error opening file for authentication.\n";
        return false;
    }

    if (!isUserExists(fileDescriptor, username)) {
        cerr << "Error: User not found.\n";
        close(fileDescriptor);
        return false;
    }

    char buffer[1024];
    ssize_t bytesRead;

    lseek(fileDescriptor, 0, SEEK_SET);

    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        char *line = strtok(buffer, "\n");

        while (line != nullptr) {

            char *colonPos = strchr(line, ':');
            if (colonPos != nullptr) {
                *colonPos = '\0';
                string userID = line;
                string password1 = colonPos + 1;

                if (userID == username && password1 == password) {
                    close(fileDescriptor);
                    return true;
                }
            }

            line = strtok(nullptr, "\n");
        }
    }

    close(fileDescriptor);
    return false;
}

bool addUser(const char *username, const char *password) {
    int fileDescriptor = open(userFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fileDescriptor == -1) {
        cerr << "Error opening file for user registration.\n";
        return false;
    }

    if (isUserExists(fileDescriptor, username)) {
        cerr << "Error: User already exists.\n";
        close(fileDescriptor);
        return false;
    }

    lseek(fileDescriptor, 0, SEEK_END);

    if (dprintf(fileDescriptor, "%s:%s\n", username, password) == -1) {
        cerr << "Error writing to file for user registration.\n";
        close(fileDescriptor);
        return false;
    }

    close(fileDescriptor);
    return true;
}

void gameSelectPage() {
    WINDOW *win5 = newwin(10, 80, 0, 0);
    WINDOW *win6 = newwin(4, 80, 10, 0);
    WINDOW *win7 = newwin(4, 80, 14, 0);
    WINDOW *win8 = newwin(4, 80, 18, 0);

    wbkgd(win5, COLOR_PAIR(1));
    wbkgd(win6, COLOR_PAIR(2));
    wbkgd(win7, COLOR_PAIR(3));
    wbkgd(win8, COLOR_PAIR(4));

    mvwprintw(win5, 5, 30, "JongHoon's GAMEPACK");
    mvwprintw(win6, 2, 20, "WORDLE GAME");
    mvwprintw(win7, 2, 20, "SNAKE GAME");
    mvwprintw(win8, 2, 20, "EXIT");
    mvwprintw(win6, 2, 10, "PRESS a");
    mvwprintw(win7, 2, 10, "PRESS s");
    mvwprintw(win8, 2, 10, "PRESS ESC");

    wrefresh(win5);
    wrefresh(win6);
    wrefresh(win7);
    wrefresh(win8);

    selectGame();
}

string chooseWord() {
    string words[] = {"apple", "blood", "plane", "grape", "melon",
                      "peach", "quiet", "table", "quite", "crime"};
    return words[rand() % (sizeof(words) / sizeof(words[0]))];
}
void displayWord(const string &guessedWord) {
    for (char letter : guessedWord) {
        printw("%c ", letter);
    }
    printw("\n");
}
void wordle() {
    clear();
    string secretWord = chooseWord();
    string guessedWord(secretWord.length(), '_');

    printw("Welcome to Wordle!\n");
    printw("Try to guess the 5-letter word.\n");

    int attempts = 0;
    while (attempts < 5) {
        printw("\nAttempts remaining: %d\n", 5 - attempts);
        displayWord(guessedWord);

        string guess;
        printw("Enter a 5-letter word: ");
        refresh();

        nodelay(stdscr, false);
        cin >> guess;
        clear();
        refresh();
        cout << "You wrote : " << guess << endl;

        if (guess.length() != 5) {
            printw("Please enter a 5-letter word.\n");
            refresh();
            continue;
        }

        for (size_t i = 0; i < 5; ++i) {
            if (guess[i] == secretWord[i]) {
                guessedWord[i] = guess[i];
            }
        }

        printw("\nMatching letters: ");
        for (size_t i = 0; i < 5; ++i) {
            if (guess[i] == secretWord[i]) {
                printw("%c ", guess[i]);
            }
        }

        printw("\nNot right place: ");
        for (size_t i = 0; i < 5; ++i) {
            if (guess[i] != secretWord[i] &&
                secretWord.find(guess[i]) != string::npos) {
                printw("%c ", guess[i]);
            }
        }

        printw("\nCorrect positions: ");
        for (size_t i = 0; i < 5; ++i) {
            if (guessedWord[i] == secretWord[i]) {
                printw("%d ", i + 1);
            }
        }

        printw("\n");
        refresh();

        if (guessedWord == secretWord) {
            printw("\nCongratulations! You guessed the word: %s\n",
                   secretWord.c_str());
            printw("If you want to go back to game select screen, press f\n");
            printw("or if you want to end this program, press ESC\n");
            afterGame();

            break;
        } else {
            printw("Incorrect! Try again.\n");
        }

        attempts++;
    }

    if (attempts == 5) {
        printw("\nSorry, you're out of attempts. The word was: %s\n",
               secretWord.c_str());
        printw("If you want to go back to game select screen, press f\n");
        printw("or if you want to end this program, press ESC\n");
        afterGame();
    }
}

void afterGame() {
    int choice;

    while (chkGameBool) {
        int c = getch();

        switch (c) {
        case 102:
            choice = 0;
            break;
        case 70:
            choice = 0;
            break;
        case 27:
            choice = 2;
        }

        if (choice == 0) {
            clear();
            refresh();
            gameSelectPage();
            gameBool = true;
            chkGameBool = false;
        } else if (choice == 2) {
            endwin();
            exit(0);
        }

        choice = -1;
    }
}

void selectGame() {
    int choice;

    while (gameBool) {
        int c = getch();

        switch (c) {
        case 97:
            choice = 0;
            break;
        case 65:
            choice = 0;
            break;
        case 83:
            choice = 1;
            break;
        case 115:
            choice = 1;
            break;
        case 27:
            choice = 2;
        }

        if (choice == 0) {
            wordle();
            gameBool = false;
        } else if (choice == 1) {
            snake();
            gameBool = false;
        } else if (choice == 2) {
            endwin();
            exit(0);
        }

        choice = -1;
    }
}

void snake() {
    SnakeGame game;
    game.play();
}

void selectMain() {
    int choice;

    while (mainBool) {
        int c = getch();

        switch (c) {
        case 49:
            choice = 0;
            break;
        case 50:
            choice = 1;
            break;
        case 27:
            choice = 2;
        }

        if (choice == 0) {
            loginPage();
        } else if (choice == 1) {

            registerPage();
        } else if (choice == 2) {
            endwin();
            exit(0);
        }

        choice = -1;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    initscr();
    if (has_colors() == FALSE) {
        perror("Terminal color error");
        endwin();
        return -1;
    } else {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_MAGENTA);
        init_pair(2, COLOR_MAGENTA, COLOR_CYAN);
        init_pair(3, COLOR_BLUE, COLOR_WHITE);
        init_pair(4, COLOR_WHITE, COLOR_BLUE);
        keypad(stdscr, TRUE);
        noecho();
    }

    refresh();

    mainPage();

    selectMain();
}
