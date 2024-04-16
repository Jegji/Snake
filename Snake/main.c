#include <curses.h>
#include <time.h>
#include <stdlib.h>
#define FPS_CAP 15

enum Direction {
	none, up, left, down, right, quit
};

struct Pos
{
	int x, y;
};

struct Snake
{
	struct Pos head;
	enum Direction lastDir;
	int length;
	struct Pos* body;
};

void initSnake(struct Snake* snake, struct Pos start);
void initBody(struct Pos* body, struct Pos head);
struct Pos* resizeBody(struct Pos* body, int lng);
int moveSnake(struct Snake* snake);
void printSnake(struct Snake snake);
void printApple(struct Pos apple);
void printSnakePos(struct Snake snake);
int checkColisionApple(struct Snake* snake, struct Pos* apple);
int checkColisionSnake(struct Snake* snake, struct Snake* ekans);
struct Pos shiftArray(struct Pos* body, int lng);
struct Pos generateApple();
enum Direction getInput();

void initCurses();

int main() {
	initCurses();
	time_t prevTime = clock();
	time_t currentTime;
	double deltaTime;
	enum Direction input = none;
	enum Direction dir = right;
	int i = 0;
	int gameover = 0;
	struct Pos start = { 3,3 };
	struct Snake snake;
	srand(time(NULL));
	struct Pos apple = generateApple();
	initSnake(&snake, start);

	while (input != quit && gameover != 1) {
		input = getInput();
		if (input != none && input != quit && input % 2 != snake.lastDir % 2) {
			dir = input;
		}

		currentTime = clock();
		deltaTime = (double)(currentTime - prevTime) / CLOCKS_PER_SEC;
		if (deltaTime > (double)1 / FPS_CAP) {
			snake.lastDir = dir;
			clear();
			if (moveSnake(&snake) || checkColisionSnake(&snake, &snake)) {
				gameover = 1;
			}
			checkColisionApple(&snake, &apple);
			printSnake(snake);
			printApple(apple);
			box(stdscr, 0, 0);
			refresh();
			prevTime = clock();
		}
	}
	//end
	endwin();
	return 0;
}

void initSnake(struct Snake* snake, struct Pos start)
{
	snake->head = start;
	snake->length = 2;
	snake->lastDir = right;
	initBody(&snake->body, snake->head);
}

void initBody(struct Pos** body, struct Pos head)
{
	*body = malloc(2 * sizeof(struct Pos));
	(*body)[0] = head;
	struct Pos tail = head;
	head.x--;
	(*body)[1] = head;
}

struct Pos* resizeBody(struct Pos* body, int lng)
{
	struct Pos* newBody = (struct Pos*)malloc(lng * sizeof(struct Pos));
	for (int i = 0; i < lng - 1; i++) {
		newBody[i] = body[i];
	}
	free(body);
	return newBody;
}

int moveSnake(struct Snake* snake)
{
	struct Pos newPos = snake->head;
	switch (snake->lastDir)
	{
	case up:
		newPos.x--;
		break;
	case down:
		newPos.x++;
		break;
	case left:
		newPos.y--;
		break;
	case right:
		newPos.y++;
		break;
	default:
		break;
	}
	shiftArray(snake->body, snake->length);
	snake->body[0] = newPos;
	snake->head = newPos;
	int maxY, maxX;
	getmaxyx(stdscr, maxX, maxY);
	if (newPos.x > 0 && newPos.x < maxX - 1) {
		if (newPos.y > 0 && newPos.y < maxY - 1) {
			return 0;
		}
	}return 1;
}

void printSnake(struct Snake snake)
{
	for (int i = 0; i < snake.length; i++) {
		move(snake.body[i].x, snake.body[i].y);
		attron(COLOR_PAIR(2));
		printw("\xe2\x96\xa0");
		attroff(COLOR_PAIR(2));
	}
}

void printApple(struct Pos apple)
{
	move(apple.x, apple.y);
	attron(COLOR_PAIR(1));
	printw("\xe2\x97\x8f");
	attroff(COLOR_PAIR(1));
}

void printSnakePos(struct Snake snake)
{
	move(0, 0);
	for (int i = 0; i < snake.length; i++) {
		printw("[%d,%d]", snake.body[i].x, snake.body[i].y);
	}
}

int checkColisionApple(struct Snake* snake, struct Pos* apple)
{
	if (snake->head.x == apple->x && snake->head.y == apple->y) {
		snake->length++;
		snake->body = resizeBody(snake->body, snake->length);
		*apple = generateApple();
		return 1;
	}return 0;
}

int checkColisionSnake(struct Snake* snake, struct Snake* ekans)
{
	for (int i = 1; i < ekans->length; i++) {
		if (snake->head.x == ekans->body[i].x && snake->head.y == ekans->body[i].y) {
			return 1;
		}
	}
	return 0;
}

struct Pos shiftArray(struct Pos* body, int lng)
{
	struct Pos last = body[lng - 1];
	for (int i = lng - 2; i >= 0; i--) {
		body[i + 1] = body[i];
	}return last;
}

struct Pos generateApple()
{
	int maxY, maxX;
	getmaxyx(stdscr, maxY, maxX);
	struct Pos apple = { rand() % (maxY - 4) + 2,rand() % (maxX - 4) + 2 };
	return apple;
}

enum Direction getInput()
{
	enum Direction dir = none;
	int ch = getch();
	if (ch != ERR) {
		switch (ch)
		{
		case 'w':
			dir = up;
			break;
		case 'a':
			dir = left;
			break;
		case 's':
			dir = down;
			break;
		case 'd':
			dir = right;
			break;
		case 'q':
			dir = quit;
			break;
		}
	}
	return dir;
}

void initCurses()
{
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	curs_set(0);
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
}
