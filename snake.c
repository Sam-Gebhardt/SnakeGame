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
	int pivot; // change in direction (causes "snake" effect)
} Node;

typedef struct data {
	struct node *head;
	struct node *tail;
	int apple_x;
	int apple_y;
} Data;

// function declerations
void cleanup(Data* lData);
char *head_of_the_snake(Data* lData);
void random_int(Data* lData);
int reversed(Data* lData, int past_x, int past_y);
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

int reversed(Data* lData, int past_x, int past_y) {

	if (lData->head->direction_x * -1 == past_x && past_x != 0) 
		return 1;
	
	if (lData->head->direction_y * -1 == past_y && past_y != 0 )
		return 1;
	
	return 0;
}

void update_direction(Data* lData) {
	Node* second = lData->head->next;

	if (second != NULL && lData->head->direction_x != second->direction_x
	 && lData->head->direction_y != second->direction_y) 
		second->pivot = 1;

	while (second != NULL) {
		if (second->pivot && second->prev != NULL) {
			second->direction_x = second->prev->direction_x;
			second->direction_y = second->prev->direction_y;
			second->pivot = 0;
		}
		second = second->next;
	}
}
	

void fix_direction(void) {
	// Is smart enough to realize that it shouldn't immedaitly go in the direction,
	// but well snake there, ideas:
	// could maintain a :pivot: that holds direction, would be costly if the snake has lots of :pivots:
	// could look for :change:, traverse ll and compare directions 
	// here it is:
	// start at the head and traverse downwards, if there is a difference in dir change it update it
	// move through the rest of the ll, O(n) is not gteat and is starting to add up, currently at O(n^3) in total 
	// but update_dir() could be modified to reduce back to n^2
	// or this: 
	// it works when size == 1, so apply same logic to each Node, ie each acts as a head to the prev
	// node in the list, 
	// ^ won't work because the head is a special case, ie, body looks to head for direction, but past the 
	// first element should look to the element before it for direction, 
	// could: apply change in direction down the list one by one
}


int move_snake(Data* lData) {

	clear();
	char *head = head_of_the_snake(lData);
	Node* current = lData->head;

	current->x_cord += current->direction_x;
	current->y_cord += current->direction_y;

	mvprintw(current->y_cord, current->x_cord, head);
	mvprintw(lData->apple_y, lData->apple_x, "@");

	if (current->x_cord == lData->apple_x && current->y_cord == lData->apple_y) 
		grow_snake(lData);

	current = current->next;
	while (current != NULL) {

		if (current->x_cord == lData->apple_x && current->y_cord == lData->apple_y)
			return 1;

		mvprintw(current->y_cord, current->x_cord, "*");
		current->x_cord += current->direction_x;
		current->y_cord += current->direction_y;
		current = current->next;

	}
	refresh();

	return 0;
}

void grow_snake(Data* lData) {
	// 1 2 3 N
	Node* new = (Node*)malloc(sizeof(Node));
	new->x_cord = lData->tail->x_cord - lData->tail->direction_x;
	new->y_cord = lData->tail->y_cord - lData->tail->direction_y;

	new->direction_x = lData->tail->direction_x;
	new->direction_y = lData->tail->direction_y;

	new->next = NULL;
	new->prev = lData->tail;

	lData->tail->next = new;
	lData->tail = new;

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

	random_int(lData);
	mvprintw(lData->apple_y, lData->apple_x, "@");

	while(1) {
		
		int move = getch();
		int change = 0;
		int past_x = lData->head->direction_x;
		int past_y = lData->head->direction_y;

		switch(move) {
			case KEY_UP:
				lData->head->direction_x = 0; 
				lData->head->direction_y = -1;
				change = 1;
				break;
			case KEY_DOWN:
				lData->head->direction_x = 0; 
				lData->head->direction_y = 1;
				change = 1;
				break;
			case KEY_RIGHT:
				lData->head->direction_x = 1; 
				lData->head->direction_y = 0;
				change = 1;
				break;
			case KEY_LEFT:
				lData->head->direction_x = -1; 
				lData->head->direction_y = 0;
				change = 1;
				break;
			case 27:  // esc
				clear();
				endwin();
				cleanup(lData);
				return 0;
		}

		if (change) 
			update_direction(lData);

		getmaxyx(stdscr, max_y, max_x); // get again incase window was resized

		if ((lData->head->x_cord > max_x || lData->head->x_cord < 0) || //fix
		(lData->head->y_cord > max_y || lData->head->y_cord < -1) || 
		(move_snake(lData)) || reversed(lData, past_x, past_y)) {

			clear();
			mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
			refresh();
			getchar();
			endwin();
			cleanup(lData);
			return 0;
		}

		if (lData->head->direction_y)
			usleep(150000);
		else 
			usleep(37500);

		refresh();

	}
	endwin();

	return 0;
}
