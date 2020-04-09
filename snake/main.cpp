#include <deque>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
using namespace std;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Point {
    int x, y;
};

inline bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

Direction dir;
bool gameOver;
const int height = 20;
const int width = 80;
deque<Point> snake;
int score;
struct Point fruit;
double delay;

inline Point newFruit() {
    return (struct Point){
        rand() % (width-2) + 1,
        rand() % (height-2) + 1
    };
}

void setup() {
	gameOver = false;
	dir = RIGHT;
    snake.push_back((struct Point){ width/2, height/2 });
    fruit = newFruit();
	score = 0;
    delay = 50000;

    initscr();
    cbreak();
    nodelay(stdscr, true);
    noecho();
    wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
}

bool onSnake(Point p) {
    for (int i = 0; i < snake.size(); i++) {
        if (snake.at(i) == p)
            return true;
    }
    return false;
}

bool isCollision() {
    Point head = snake.back();
    for (int i = 0; i < snake.size()-1; i++) {
        if (snake.at(i) == head)
            return true;
    }
    return false;
}

void draw() {
    clear();

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
            char ch;
			if (col == 0 || col == width-1 || row == 0 || row == height-1)
				ch = '#';
            else if (onSnake((struct Point){col, row}))
                ch = 'o';
            else if (row == fruit.y && col == fruit.x)
                ch = 'F';
            else
                ch = ' ';
            mvaddch(row, col, ch);
		}
	}

    refresh();
}

void input() {
    int key = getch();
    switch (key) {
        case 'a':
            dir = LEFT;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 'x':
            gameOver = true;
            break;
    }
}

inline void shiftCoord(int* coord, int addend, int min, int max) {
    int shifted = *coord + addend;
    if (shifted < min || shifted > max)
        shifted = addend > 0 ? min : max;
    *coord = shifted;
}

Point nextHead() {
    Point next = snake.back();
    int* member;
    int addend, min = 1, max;

    switch (dir) {
        case LEFT:
            member = &next.x;
            addend = -1;
            max = width-2;
            break;
        case UP:
            member = &next.y;
            addend = -1;
            max = height-2;
            break;
        case RIGHT:
            member = &next.x;
            addend = 1;
            max = width-2;
            break;
        case DOWN:
            member = &next.y;
            addend = 1;
            max = height-2;
            break;
        default:
            break;
    }
    shiftCoord(member, addend, min, max);

    return next;
}

void moveFruit() {
    do {
        fruit = newFruit();
    } while (onSnake(fruit));
}

void logic() {
    Point prevHead = snake.back();
    snake.push_back(nextHead());
    if (!(prevHead == fruit))
        snake.pop_front();
    else
        moveFruit();
    if (isCollision())
        gameOver = true;
}

void start() { }

int main() {
	setup();
	while (!gameOver) {
		draw();
        input();
        logic();
        usleep(delay);
	}
	return 0;
}
