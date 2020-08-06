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
	int direction_x;
	int direction_y;
} Node;

typedef struct data {
	struct node *head;
	struct node *tail;
	int apple_x;
	int apple_y;
	int apple;
} Data;

// function declerations
void cleanup(Data* lData);
char *head_of_the_snake(Data* lData);
void random_int(Data* lData);
void update_direction(Data* lData);
int move_snake(Data* lData);
void grow_snake(Data* lData);
Data* create_snake(int max_x, int max_y);
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

char *head_of_the_snake(Data* lData) {
	char *head;

	switch(lData->head->direction_x){
		case 1:
			head = ">";
			break;
		case -1:
			head = "<";
			break;
	}
	switch(lData->head->direction_y) {
		case 1:
			head = "v";
			break;
		case -1:
			head = "^";
			break;
	}
	return head;
}


void random_int(Data* lData) {
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	srand(time(NULL));
	lData->apple_x = (rand() % max_x);
	lData->apple_y = (rand() % max_y);
}

void update_direction(Data* lData) {
	Node* current = lData->head->next;

	while (current != NULL) {
		if (current->direction_x != current->prev->direction_x &&
		 current->direction_y != current->prev->direction_y) {

			 current->direction_x = current->prev->direction_x;
			 current->direction_y = current->prev->direction_y;
		 }

		current = current->next;
	}
}


int move_snake(Data* lData) {
	// mvprintw(10, 10, "x:%d, y:%d", lData->tail->x_cord, lData->tail->y_cord);
	char *head = head_of_the_snake(lData);
	mvprintw(lData->head->x_cord + lData->head->direction_x,
	 lData->head->y_cord + lData->head->direction_y, head);

	return 0;
}

void grow_snake(Data* lData) {

	// Node* new = (Node*)malloc(sizeof(Node));

	
	random_int(lData); // put another apple on the screen
}


Data* create_snake(int max_x, int max_y) {
	Node* head = (Node*)malloc(sizeof(Node));
	Data* lData = (Data*)malloc(sizeof(Data));

	lData->head = head;
	lData->tail = head;

	head->direction_x = 1;
	head->direction_y = 0;

	head->x_cord = max_x / 2;
	head->y_cord = max_y / 2;

	head->prev = NULL;
	head->next = NULL;

	return lData;
}

int main() {

	int max_x, max_y;
	int uneaten_apple = 0;

	initscr();
	noecho();  // no keyboard input
	curs_set(FALSE);
	keypad(stdscr, TRUE);

	getmaxyx(stdscr, max_y, max_x);
	Data* lData = create_snake(max_x, max_y);

	mvprintw(max_y / 2 - 1, max_x / 2 - 7, "Classic Snake");
	mvprintw(max_y / 2, max_x / 2, ">");

	refresh();
	getchar();  // wait for user to press a key
	nodelay(stdscr, true);
	clear();

	while(1) {
		
		int move = getch();

		switch(move) {
			case KEY_UP:
				lData->head->direction_x = 0; 
				lData->head->direction_y = -1;
				break;
			case KEY_DOWN:
				lData->head->direction_x = 0; 
				lData->head->direction_y = 1;
				break;
			case KEY_RIGHT:
				lData->head->direction_x = 1; 
				lData->head->direction_y = 0;
				break;
			case KEY_LEFT:
				lData->head->direction_x = -1; 
				lData->head->direction_y = 0;
				break;
			case 27:  // esc
				clear();
				endwin();
				return 0;
		}
	
		getmaxyx(stdscr, max_y, max_x); // get again incase window was resized

		if ((lData->head->x_cord > max_x || lData->head->x_cord < 0) || //fix
		(lData->head->y_cord > max_y || lData->head->y_cord < -1) || 
		(move_snake(lData))) {

			clear();
			mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
			refresh();
			getchar();
			endwin();
			cleanup(lData);
			return 0;
		}

		if (lData->head->direction_y) {
			usleep(150000);
		} else {
			usleep(37500);
		}
		if (! uneaten_apple) {
			uneaten_apple = 1;
			random_int(lData);
			mvprintw(lData->apple_y, lData->apple_x, "@");
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