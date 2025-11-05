
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <deque>
#include <ctime>
#include <cstdlib>
#include <string> 

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;


enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum GameState { STATE_START, STATE_PLAYING, STATE_GAME_OVER };

HANDLE hStdout;

struct SnakeGame {
    deque<pair<int, int>> snake;
    pair<int, int> fruit;
    Direction dir;
    GameState state;
    bool exitRequest;
    int score;
    int speedMs;

    
    CHAR_INFO screen[ (HEIGHT + 4) * (WIDTH + 4) ];
    COORD bufferSize;
    SMALL_RECT writeRegion;

    SnakeGame() {
        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        bufferSize = { (SHORT)(WIDTH + 4), (SHORT)(HEIGHT + 4) };
        writeRegion = { 0, 0, (SHORT)(WIDTH + 3), (SHORT)(HEIGHT + 3) };
        
        
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hStdout, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(hStdout, &cursorInfo);

        
        srand((unsigned)time(nullptr));
        
        state = STATE_START; 
        exitRequest = false;
        reset(); 
    }

    void reset() {
        snake.clear();
        int startX = WIDTH / 2;
        int startY = HEIGHT / 2;
        snake.push_back({ startX, startY });
        snake.push_back({ startX - 1, startY });
        snake.push_back({ startX - 2, startY });
        
        dir = RIGHT;
        placeFruit();
        score = 0;
        speedMs = 100;
       
    }

    void placeFruit() {
        while (true) {
            int fx = rand() % WIDTH;
            int fy = rand() % HEIGHT;
            bool coll = false;
            for (auto &p : snake)
                if (p.first == fx && p.second == fy) { coll = true; break; }
            if (!coll) { fruit = { fx, fy }; break; }
        }
    }

    
    void writeStringToBuffer(string s, int x, int y, WORD attributes) {
        for (size_t i = 0; i < s.length(); ++i) {
            if (x + i >= 0 && x + i < (WIDTH + 4)) {
                int index = y * (WIDTH + 4) + (x + i);
                if (index >= 0 && index < (HEIGHT + 4) * (WIDTH + 4)) {
                    screen[index].Char.AsciiChar = s[i];
                    screen[index].Attributes = attributes;
                }
            }
        }
    }

    void drawToBuffer() {
      
        for (int i = 0; i < (HEIGHT + 4) * (WIDTH + 4); ++i) {
            screen[i].Char.AsciiChar = ' ';
            screen[i].Attributes = FOREGROUND_GREEN;
        }

        
        switch (state) {
            case STATE_START: {
                string title = "SNAKE GAME";
                string start = "Press any key to start";
                writeStringToBuffer(title, (WIDTH + 4 - title.length()) / 2, HEIGHT / 2 - 2, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                writeStringToBuffer(start, (WIDTH + 4 - start.length()) / 2, HEIGHT / 2, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            }

            case STATE_PLAYING:
            case STATE_GAME_OVER: { 
            
                for (int x = 0; x < WIDTH + 2; ++x) {
                    screen[x].Char.AsciiChar = '#';
                    screen[x].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
                    screen[(HEIGHT + 1) * (WIDTH + 4) + x].Char.AsciiChar = '#';
                    screen[(HEIGHT + 1) * (WIDTH + 4) + x].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
                }
                for (int y = 0; y < HEIGHT + 2; ++y) {
                    screen[y * (WIDTH + 4)].Char.AsciiChar = '#';
                    screen[y * (WIDTH + 4)].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
                    screen[y * (WIDTH + 4) + WIDTH + 1].Char.AsciiChar = '#';
                    screen[y * (WIDTH + 4) + WIDTH + 1].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
                }

                
                int fx = fruit.first + 1;
                int fy = fruit.second + 1;
                screen[fy * (WIDTH + 4) + fx].Char.AsciiChar = 'O';
                screen[fy * (WIDTH + 4) + fx].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;

                
                for (size_t i = 0; i < snake.size(); ++i) {
                    int sx = snake[i].first + 1;
                    int sy = snake[i].second + 1;
                    screen[sy * (WIDTH + 4) + sx].Char.AsciiChar = (i == 0 ? 'X' : 'x');
                    screen[sy * (WIDTH + 4) + sx].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                }

                
                string s = "Score: " + to_string(score) + " (Q to Quit)";
                writeStringToBuffer(s, 1, HEIGHT + 2, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

                
                if (state == STATE_GAME_OVER) {
                    string over = "GAME OVER! Final Score: " + to_string(score);
                    string restart = "Press 'R' to play again or 'Q' to quit.";
                    
                    writeStringToBuffer(over, (WIDTH + 4 - over.length()) / 2, HEIGHT / 2 - 1, FOREGROUND_RED | FOREGROUND_INTENSITY);
                    writeStringToBuffer(restart, (WIDTH + 4 - restart.length()) / 2, HEIGHT / 2 + 1, FOREGROUND_RED | FOREGROUND_INTENSITY);
                }
                break;
            }
        }

        
        WriteConsoleOutputA(hStdout, screen, bufferSize, {0, 0}, &writeRegion);
    }

    void input() {
        if (_kbhit()) {
            int ch = _getch();

            switch (state) {
                case STATE_START:
                    
                    reset();
                    state = STATE_PLAYING;
                    break;
                
                case STATE_PLAYING:
                    if (ch == 0 || ch == 224) { 
                        int code = _getch();
                        if (code == 75 && dir != RIGHT) dir = LEFT;
                        else if (code == 77 && dir != LEFT) dir = RIGHT;
                        else if (code == 72 && dir != DOWN) dir = UP;
                        else if (code == 80 && dir != UP) dir = DOWN;
                    } else { 
                        if ((ch == 'a' || ch == 'A') && dir != RIGHT) dir = LEFT;
                        else if ((ch == 'd' || ch == 'D') && dir != LEFT) dir = RIGHT;
                        else if ((ch == 'w' || ch == 'W') && dir != DOWN) dir = UP;
                        else if ((ch == 's' || ch == 'S') && dir != UP) dir = DOWN;
                        else if (ch == 'q' || ch == 'Q') state = STATE_GAME_OVER;
                    }
                    break;

                case STATE_GAME_OVER:
                    if (ch == 'r' || ch == 'R') {
                        state = STATE_START;
                        reset();
                    }
                    else if (ch == 'q' || ch == 'Q') {
                        exitRequest = true; 
                    }
                    break;
            }
        }
    }

    void logic() {
        
        if (state != STATE_PLAYING) {
           
            speedMs = 50; 
            return;
        }

        
        speedMs = max(40, 100 - (score / 10) * 3);

        pair<int,int> head = snake.front();
        pair<int,int> newHead = head;
        if (dir == LEFT) newHead.first--;
        if (dir == RIGHT) newHead.first++;
        if (dir == UP) newHead.second--;
        if (dir == DOWN) newHead.second++;

       
        bool collision = false;
       
        if (newHead.first < 0 || newHead.first >= WIDTH || newHead.second < 0 || newHead.second >= HEIGHT)
            collision = true;

       
        for (auto &p : snake)
            if (p == newHead)
                collision = true;

        if (collision) {
            state = STATE_GAME_OVER; 
            return;
        }

        snake.push_front(newHead);

       
        if (newHead == fruit) {
            score += 10;
            placeFruit();
        } else {
            snake.pop_back();
        }
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    SnakeGame game;

    
    while (!game.exitRequest) {
        game.input();  
        game.logic();  
        game.drawToBuffer(); 
        Sleep(game.speedMs); 
    }

    
    system("cls");
    cout << "Thanks for playing!";
    Sleep(1200);
    
    return 0;
}