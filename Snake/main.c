#include <curses.h>
#include <time.h>
#include <stdlib.h>
#define FPS_CAP 25

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
	int col;
	struct Pos* body;
};

struct SnakeArray {
	struct Snake** array;
	int lng;
};

void initSnake(struct Snake* snake, struct Pos start);
void initBody(struct Pos* body, struct Pos head);
void initSnakeArray(struct SnakeArray* arr);
void pushSnake(struct SnakeArray* arr, struct Snake* snake);
int removeSnake(struct SnakeArray* arr, struct Snake* snake);
struct Pos* resizeBody(struct Pos* body, int lng);
int moveSnake(struct Snake* snake);
int moveSnakes(struct SnakeArray* arr);
void printSnake(struct Snake snake);
void printSnakes(struct SnakeArray* arr);
void printApple(struct Pos apple);
void printSnakePos(struct Snake snake);
int checkColisionApple(struct SnakeArray* arr, struct Pos* apple);
int checkColisionSnake(struct Snake* snake, struct Snake* ekans);
void chceckColisionSnakes(struct SnakeArray* arr);
struct Pos shiftArray(struct Pos* body, int lng);
struct Pos generateApple();
enum Direction getInput();

void initCurses();

int main() {
	initCurses();
	srand(time(NULL));
	time_t prevTime = clock();
	time_t currentTime;
	double deltaTime;
	int gameover = 0;

	enum Direction input = none;
	enum Direction dir = right;
	struct Pos start = { 3,3 };
	struct Snake player;
	initSnake(&player, start);

	start.y += 10;
	struct Snake ekans;
	initSnake(&ekans, start);

	struct SnakeArray snakeArray;
	initSnakeArray(&snakeArray);
	pushSnake(&snakeArray, &player);
	pushSnake(&snakeArray, &ekans);

	struct Pos apple = generateApple(&snakeArray);
	
	while (input != quit && gameover != 1 && snakeArray.lng != 0) {
		input = getInput();
		if (input != none && input != quit && input % 2 != player.lastDir % 2) {
			dir = input;
		}

		currentTime = clock();
		deltaTime = (double)(currentTime - prevTime) / CLOCKS_PER_SEC;
		if (deltaTime > (double)1 / FPS_CAP) {
			player.lastDir = dir;
			clear();
			moveSnakes(&snakeArray);
			chceckColisionSnakes(&snakeArray);
			checkColisionApple(&snakeArray, &apple);
			printSnakes(&snakeArray);
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
	static int color = 2;
	snake->col = color;
	init_pair(color, color, color+1);
	color++;
	snake->head = start;
	snake->length = 2;
	snake->lastDir = right;
	initBody(&snake->body, snake->head);
}

void initSnakeArray(struct SnakeArray* arr)
{
	arr->lng = 0;
	arr->array = NULL;
}

void pushSnake(struct SnakeArray* arr, struct Snake* snake)
{
	arr->lng++;
	struct Snake** newArr = malloc(arr->lng * sizeof(struct Snake*));
	for (int i = 0; i < arr->lng - 1; i++) {
		newArr[i] = arr->array[i];
	}free(arr->array);
	newArr[arr->lng - 1] = snake;
	arr->array = newArr;
}

int removeSnake(struct SnakeArray* arr,struct Snake* snake)
{
	for (int i = 0; i < arr->lng; i++) {
		if (arr->array[i] == snake) {
			struct Snake** newArr = malloc((arr->lng - 1) * sizeof(struct Snake*));
			for (int j = 0; j < i; j++) {
				newArr[j] = arr->array[j];
			}
			for (int j = i + 1; j < arr->lng; j++) {
				newArr[j - 1] = arr->array[j];
			}
			free(arr->array);
			arr->array = newArr;
			arr->lng--;
			return 1;
		}
	}return 0;
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

int moveSnakes(struct SnakeArray* arr)
{
	int counter = 0;
	for (int i = 0; i < arr->lng; i++) {
		if (moveSnake(arr->array[i])) {
			removeSnake(arr,arr->array[i]);
		}
	}
	//return counter; //returns the numbers of colision with wall of all snakes
	return 0; // doing nothing
}

void printSnake(struct Snake snake)
{
	for (int i = 0; i < snake.length; i++) {
		move(snake.body[i].x, snake.body[i].y);
		attron(COLOR_PAIR(snake.col));
		printw("\xe2\x96\xa0");
		attroff(COLOR_PAIR(snake.col));
	}
}

void printSnakes(struct SnakeArray* arr)
{
	for (int i = 0; i < arr->lng; i++) {
		printSnake(*arr->array[i]);
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

int checkColisionApple(struct SnakeArray* arr, struct Pos* apple)
{
	for (int i = 0; i < arr->lng; i++) {
		if (arr->array[i]->head.x == apple->x && arr->array[i]->head.y == apple->y) {
			arr->array[i]->length++;
			arr->array[i]->body = resizeBody(arr->array[i]->body, arr->array[i]->length);
			*apple = generateApple(arr);
			return 1;
		}
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

void chceckColisionSnakes(struct SnakeArray* arr)
{
	int* list = malloc(arr->lng * sizeof(int));
	for (int i = 0; i < arr->lng; i++) {
		list[i] = 0;
	}
	for (int i = 0; i < arr->lng; i++) {
		for (int j = i; j < arr->lng; j++) {
			if (arr->array[i] != arr->array[j]) {
				for (int k = 0; k < arr->array[j]->length; k++) {
					if (arr->array[i]->head.x == arr->array[j]->body[k].x && arr->array[i]->head.y == arr->array[j]->body[k].y) {
						list[i] = 1;
					}
				}
			}
			else {
				if (checkColisionSnake(arr->array[i], arr->array[i])) {
					list[i] = 1;
				}
			}
		}
	}
	for (int i = 0; i < arr->lng; i++) {
		if (list[i]) {
			removeSnake(arr, arr->array[i]);
		}
	}
}

struct Pos shiftArray(struct Pos* body, int lng)
{
	struct Pos last = body[lng - 1];
	for (int i = lng - 2; i >= 0; i--) {
		body[i + 1] = body[i];
	}return last;
}

int appleonSnake(struct SnakeArray* arr,struct Pos pos) {
	for (int i = 0; i < arr->lng; i++) {
		for (int j = 0; j < arr->array[i]->length; j++) {
			if (pos.x == arr->array[i]->body[j].x && pos.y == arr->array[i]->body[j].y) {
				return 1;
			}
		}
	}return 0;
}

struct Pos generateApple(struct SnakeArray* arr)
{
	int maxY, maxX;
	getmaxyx(stdscr, maxY, maxX);
	struct Pos apple;
	do {
		apple.x = rand() % (maxY - 4) + 2;
		apple.y = rand() % (maxX - 4) + 2;
	} while (appleonSnake(arr, apple));
	
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
}