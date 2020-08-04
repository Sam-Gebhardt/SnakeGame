/*
Author: Sam Gebhardt
Basic terminal snake game
*/

#include <stdio.h>
// #include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

typedef struct node {
	struct node *next;
	struct node *prev;
	int x_cord;
	int y_cord;
} Node;

typedef struct data {
	struct node *head;
	struct node *tail;
	int apple_x;
	int apple_y;
} Data;

// function declerations
void cleanup(Data* lData);
char *head_of_the_snake(int direction_x, int direction_y);
void random_int(int out[2], int max_x, int max_y);
int move_snake(Data* lData, int direction_x, int direction_y);
void create_snake(Data* lData, int max_x, int max_y);
void grow_snake(Data* lData);
void tail_direction(Data* lData);
// function declerations


void cleanup(Data* lData) {
	Node* current = lData->head;
	Node* next;

	while (current != NULL) {
		next = current->next;
		free((void *)current);
		current = next;
	}
	free(lData);
}

char *head_of_the_snake(int direction_x, int direction_y) {
	char *head;

	switch(direction_x){
		case 1:
			head = ">";
			break;
		case -1:
			head = "<";
			break;
	}
	switch(direction_y) {
		case 1:
			head = "v";
			break;
		case -1:
			head = "^";
			break;
	}
	return head;
}


void random_int(int out[2], int max_x, int max_y) {
	
	srand(time(NULL));
	out[0] = (rand() % max_x);
	out[1] = (rand() % max_y);
}

// int snake_collison(Data* lData) {
// 	Node* node = lData->head->next;

// 	while (node->next != NULL) {
// 		if (node->x_cord == lData->head->x_cord && node->y_cord == lData->head->y_cord) {
// 			return 1;  // snake tried to eat itself
// 		}
// 		node = node->next;
// 	}
// 	return 0;
// }

int move_snake(Data* lData, int direction_x, int direction_y) {
	// Replace the head car with a body char, move the head based on input, check if an apple was eaten,
	// Move the head and remove tail
	Node* node = lData->head;

	mvprintw(lData->head->y_cord, lData->head->x_cord, "*");
	// mvprintw(lData->head->y_cord, lData->head->x_cord, "*");

	while (node->next != NULL) {
		node->x_cord += direction_x;
		node->y_cord += direction_y;

		if (node == lData->head || node == lData->tail) {
			node = node->next;
			continue;
		}
		if (lData->head->x_cord == node->x_cord && lData->head->y_cord && node->y_cord) 
			return 1;
		if (lData->head->x_cord == lData->apple_x && lData->head->y_cord == lData->apple_y) {
			grow_snake(lData);
		} // dont want to do this each time
	
		node = node->next;
	}

	char *head = head_of_the_snake(direction_x, direction_y);
	mvprintw(lData->head->y_cord, lData->head->x_cord, head);
	mvprintw(lData->tail->y_cord, lData->tail->x_cord, " ");
	refresh();

	lData->tail->x_cord += direction_x;
	lData->tail->y_cord += direction_y;

	return 0;
}

void grow_snake(Data* lData) {
	Node* new = (Node*)malloc(sizeof(Node));

	lData->tail->next = new;
	new->prev = lData->tail;
	new->next = NULL;
	new->x_cord = lData->tail->x_cord - 1;
	new->y_cord = lData->tail->y_cord;

	lData->tail = new;

}


void create_snake(Data* lData, int max_x, int max_y) {

	Node *snake_head = (Node*)malloc(sizeof(Node));
	Node *snake_tail = (Node*)malloc(sizeof(Node));

	snake_head->x_cord = max_x / 2;
	snake_head->y_cord = max_y / 2;

	snake_tail->x_cord = max_x / 2 - 1;
	snake_tail->y_cord = max_y / 2;

	snake_head->next = snake_tail;
	snake_head->prev = NULL;

	snake_tail->next = NULL;
	snake_tail->prev = snake_head;

	lData->head = snake_head;
	lData->tail = snake_tail;
}

int main() {

	int x, y, max_x, max_y;
	int direction_x = 1, direction_y = 0;
	int uneaten_apple = 0;

	initscr();
	noecho();  // no keyboard input
	curs_set(FALSE);
	keypad(stdscr, TRUE);

	getmaxyx(stdscr, max_y, max_x);
	Data* lData = (Data*)malloc(sizeof(Data));
	create_snake(lData, max_x, max_y);

	lData->tail->x_cord = max_x / 2;
	lData->tail->y_cord = max_y / 2;

	mvprintw(max_y / 2 - 1, max_x / 2 - 7, "Classic Snake");
	mvprintw(max_y / 2, max_x / 2, ">");
	refresh();

	x = max_x / 2;
	y = max_y / 2;

	lData->head->x_cord = x;
	lData->head->y_cord = y;

	refresh();
	getchar();  // wait for user to press a key
	nodelay(stdscr, true);
	clear();

	while(1) {

		int apple[2];
		int move = getch();

		switch(move) {
			case KEY_UP:
				direction_x = 0; direction_y = -1;
				break;
			case KEY_DOWN:
				direction_x = 0; direction_y = 1;
				break;
			case KEY_RIGHT:
				direction_x = 1; direction_y = 0;
				break;
			case KEY_LEFT:
				direction_x = -1; direction_y = 0;
				break;
			case 27:  // esc
				clear();
				endwin();
				return 0;
		}
	
		getmaxyx(stdscr, max_y, max_x); // get again incase window was resized

		// if (move_snake(lData, direction_x, direction_y)) {
		// 	clear();
		// 	mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
		// 	refresh();
		// 	getchar();
		// 	endwin();
		// 	return 0;
		// }

		if ((lData->head->x_cord > max_x || lData->head->x_cord < 0) || //fix
		(lData->head->y_cord > max_y || lData->head->y_cord < -1) || 
		(move_snake(lData, direction_x, direction_y))) {

			clear();
			mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
			refresh();
			getchar();
			endwin();
			cleanup(lData);
			return 0;
		}

		if (direction_y) {
			usleep(150000);
		} else {
			usleep(37500);
		}
		if (! uneaten_apple) {
			uneaten_apple = 1;
			random_int(apple, max_x, max_y);
			mvprintw(apple[1], apple[0], "@");
			lData->apple_x = apple[0];
			lData->apple_y = apple[1];
		}
		refresh();

	}
	endwin();

	return 0;
}


/*
Game Loop:
Start in middle of screen with direction (1, 0)
Put "apple" on the screen for the snake to eat
Take user input(3 cases)-
	a. inbounds
	b. out of bounds(border or snake itself)
	c. ate apple

a: update head of snake and move snake body
b. Display "Game over" and score
c. Increase score by 1 and snake body by 1

Hold snake body in linked list?
	doubly liked list-
	allows movement in constent time(Move last * to the front)
	add len by adding * after head of the snake
	checking if the snake ate itself by looping though ll
		not ideal with O(n), but the only way I think
*/

// TODO: add direction x/y to lData, turn into singly linked list