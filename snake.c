/*
Author: Sam Gebhardt
Basic terminal snake game
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

typedef struct node {
	struct node *next;
	struct node *prev;
	int x_cord;
	int y_cord;
	int x_direction;
	int y_direction;
	int pivot; // change in direction (causes "snake" effect)
} Node;

typedef struct data {
	struct node *head;
	struct node *tail;
	int score; 
	int x_apple;
	int y_apple;
} Data;

// function declerations
void free_list(Data* lData);
char *head_of_the_snake(Data* lData);
void gen_apple(Data* lData);
int backwards(Data* lData, int past_x, int past_y);
void update_direction(Data* lData);
void move_snake(Data* lData);
void grow_snake(Data* lData);
int collison(Data* lData, int past_x, int past_y);
Data* create_snake(int max_x, int max_y);
// function declerations


void free_list(Data* lData) {
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

	switch(lData->head->x_direction){
		case 1:
			head = ">";
			break;
		case -1:
			head = "<";
			break;
	}
	switch(lData->head->y_direction) {
		case 1:
			head = "v";
			break;
		case -1:
			head = "^";
			break;
	}
	return head;
}


void gen_apple(Data* lData) {
	Node* current = lData->head->next;
	int max_y, max_x, repick = 0;
	getmaxyx(stdscr, max_y, max_x);
	
	while (1) { 
		/* 
		keep looping till a valid cord pair is found
		Very inefficent as the snake grows larger, TODO: make a faster/better 
		method to spawn apples on the screen.
		*/

		srand(time(NULL));
		lData->x_apple = (rand() % max_x);
		lData->y_apple = (rand() % max_y);

		while (current != NULL) {
			if (current->x_cord == lData->x_apple && current->y_cord == lData->y_apple) {
				repick = 1;
				break;
			}
			
			current = current->next;
		}

		if (! repick)
			break;
	}


}

int backwards(Data* lData, int past_x, int past_y) {

	if (lData->head->x_direction * -1 == past_x && past_x != 0) 
		return 1;
	
	if (lData->head->y_direction * -1 == past_y && past_y != 0 )
		return 1;
	
	return 0;
}

void update_direction(Data* lData) {

	if (lData->head->next == NULL)
		return;
	
	lData->head->next->x_direction = lData->head->x_direction;
	lData->head->next->y_direction = lData->head->y_direction;

	if (lData->head->next->next != NULL) {
		lData->head->next->next->pivot = 1;
	}
}


void move_snake(Data* lData) {

	clear();
	char *head = head_of_the_snake(lData);
	Node* current = lData->head;
	int x = lData->head->x_cord, y = lData->head->y_cord;
	int max_x;

	max_x = getmaxx(stdscr);

	current->x_cord += current->x_direction;
	current->y_cord += current->y_direction;

	mvprintw(current->y_cord, current->x_cord, head);
	mvprintw(lData->y_apple, lData->x_apple, "@");
	mvprintw(0, max_x - 3, "%d", lData->score); // current score

	if (current->x_cord == lData->x_apple && current->y_cord == lData->y_apple) 
		grow_snake(lData);

	current = current->next;
	while (current != NULL) {

		if (current != lData->head->next && ! current->pivot) {
			x -= current->prev->x_direction;
			y -= current->prev->y_direction;
			current->x_cord = x;
			current->y_cord = y;
		} else if (current->pivot == 1) {
			x -= current->x_direction;
			y -= current->y_direction;
			current->x_cord += current->x_direction;
			current->y_cord += current->y_direction;
			current->pivot ++;
		} else if (current->pivot == 2) {
			x -= current->prev->x_direction;
			y -= current->prev->y_direction;
			current->x_cord = x;
			current->y_cord = y;
			current->x_direction = current->prev->x_direction;
			current->y_direction = current->prev->y_direction;
			current->pivot = 0;
			if (current->next != NULL)
				current->next->pivot = 1;
		}

		/*
		TODO: 
		Change in directions arent perfect 90 degress
		moving parrell to the body shifts the whole snake up/down
		*/

		mvprintw(y, x, "*");
		current = current->next;

	}
	refresh();
}

void grow_snake(Data* lData) {
	
	Node* new = (Node*)malloc(sizeof(Node));
	new->x_cord = lData->tail->x_cord - lData->tail->x_direction;
	new->y_cord = lData->tail->y_cord - lData->tail->y_direction;

	new->x_direction = lData->tail->x_direction;
	new->y_direction = lData->tail->y_direction;

	new->next = NULL;
	new->prev = lData->tail;

	lData->tail->next = new;
	lData->tail = new;
	lData->score ++;

	gen_apple(lData); // put another apple on the screen
}

int collion(Data* lData, int past_x, int past_y) {
	// check to see if the move is valid
	int max_x, max_y;
	Node* head = lData->head;
	Node* current = lData->head->next;

	getmaxyx(stdscr, max_y, max_x); // get again incase window was resized

	while (current != NULL) {

		if (current == lData->head->next) {
			current = current->next;
			continue;
		}

		if (current->x_cord == head->x_cord && current->y_cord == head->y_cord)
			return 1;

		current = current->next;
	}

	return lData->head->x_cord >= max_x || lData->head->x_cord < 0 || 
		lData->head->y_cord >= max_y || lData->head->y_cord < 0 || 
		backwards(lData, past_x, past_y);
}

Data* create_snake(int max_x, int max_y) {
	Node* head = (Node*)malloc(sizeof(Node));
	Data* lData = (Data*)malloc(sizeof(Data));

	lData->head = head;
	lData->tail = head;
	lData->score = 0;

	head->x_direction = 1;
	head->y_direction = 0;

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

	gen_apple(lData);
	mvprintw(lData->y_apple, lData->x_apple, "@");

	while(1) {
		
		int move = getch();
		int change = 0;
		int past_x = lData->head->x_direction;
		int past_y = lData->head->y_direction;

		switch(move) {
			case 'w':
			case KEY_UP:
				lData->head->x_direction = 0; 
				lData->head->y_direction = -1;
				change = 1;
				break;
			case 's':
			case KEY_DOWN:
				lData->head->x_direction = 0; 
				lData->head->y_direction = 1;
				change = 1;
				break;
			case 'd':
			case KEY_RIGHT:
				lData->head->x_direction = 1; 
				lData->head->y_direction = 0;
				change = 1;
				break;
			case 'a':
			case KEY_LEFT:
				lData->head->x_direction = -1; 
				lData->head->y_direction = 0;
				change = 1;
				break;
			case 27:  // esc
				clear();
				endwin();
				free_list(lData);
				return 0;
		}

		if (change)
			update_direction(lData);
		
		move_snake(lData);

		if (collion(lData, past_x, past_y)) {

			clear();
			mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
			mvprintw(max_y / 2 + 1, max_x / 2 - 4, "Score: %d", lData->score);
			refresh();
			getchar();
			endwin();
			free_list(lData);
			return 0;
		}

		if (lData->head->y_direction)
			usleep(150000);
			// usleep(300000);

		else 
			usleep(37500);
			// usleep(300000);

		refresh();

	}
	endwin();

	return 0;
}

//TODO: add highscore w/ persistant memory
