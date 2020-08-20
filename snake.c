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
void high_score(Data* lData);
void process_color(char snake_c[10], char apple_c[10], char background_c[10]);
void custom_color();
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

void high_score(Data* lData) {

	FILE *f;
	int high, x, y, new = 0;

	if (access(".highscore.txt", F_OK == -1)) { //file does not exsist
		f = fopen(".highscore.txt", "w");
		fprintf(f, "%d", lData->score);

	} else {									//file does exsist
		f = fopen(".highscore.txt", "r+");
		fscanf(f, "%d", &high);
		fclose(f);

		if (lData->score > high) {
			f = fopen(".highscore.txt", "w+");
			fprintf(f, "%d", lData->score);
			new = 1;
		} 

		getmaxyx(stdscr, y, x);
		char *message = (new) ?  "New high score!: %d" : "High Score: %d";
		int displacement = (new) ? 10 : 7;
		int score = (new) ? lData->score : high;

		mvprintw((y / 2) + 2, (x / 2) - displacement, message, score);

	}
	if (f != NULL)
		fclose(f);
}

void process_color(char snake_c[10], char apple_c[10], char background_c[10]) {
	switch (snake_c) {
		case "black":
		case "yellow":
		case "red":
		case "green":
		case "magenta":
		case "cyan":
		case "blue":
		case "white":

	}
	printf("Color doesn't exsist. Using default colors.");
}

void custom_color() {
	int max_y;
	char snake_c[10], apple_c[10], background_c[10];

	max_y = getmaxy(stdscr);
	clear();
	attron(COLOR_PAIR(1));
	mvprintw(max_y / 2, 0, 
	"Pick a color(for snake):\nblack \nyellow \nred \ngreen \nmagenta \ncyan \nblue \nwhite");
	getstr(snake_c);

	mvprintw(max_y / 2, 0, 
	"Pick a color(for apple):\nblack \nyellow \nred \ngreen \nmagenta \ncyan \nblue \nwhite");
	getstr(apple_c);

	mvprintw(max_y / 2, 0, 
	"Pick a color(for background):\nblack \nyellow \nred \ngreen \nmagenta \ncyan \nblue \nwhite");
	getstr(background_c);

	refresh();

	attroff(COLOR_PAIR(1));

	process_color(snake_c, apple_c, background_c);

	getchar();
}

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

	attron(COLOR_PAIR(2));
	mvprintw(lData->y_apple, lData->x_apple, "@");
	attroff(COLOR_PAIR(2));

	attron(COLOR_PAIR(3));
	mvprintw(0, max_x - 3, "%d", lData->score); // current score
	attroff(COLOR_PAIR(3));

	if (current->x_cord == lData->x_apple && current->y_cord == lData->y_apple) 
		grow_snake(lData);
	
	attron(COLOR_PAIR(1));
	mvprintw(current->y_cord, current->x_cord, head);
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
	attroff(COLOR_PAIR(1));
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
	int max_x, max_y, click;

	initscr();
	noecho();  // no keyboard input
	curs_set(FALSE);
	keypad(stdscr, TRUE);

	if (has_colors() == FALSE) {
		endwin();
		printf("Your terminal doen't support color.\n");
		return 1;
	}

	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_BLACK, COLOR_BLACK);
	wbkgd(stdscr, COLOR_PAIR(4));

	getmaxyx(stdscr, max_y, max_x);
	Data* lData = create_snake(max_x, max_y);

	attron(COLOR_PAIR(3));
	mvprintw(max_y / 2 - 1, max_x / 2 - 7, "Classic Snake");
	mvprintw(max_y / 2, max_x / 2, ">");
	attroff(COLOR_PAIR(3));

	refresh();
	click = getchar();  // wait for user to press a key
	if (click == 's') {
		custom_color();
	}
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

			attron(COLOR_PAIR(3));
			mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
			mvprintw(max_y / 2 + 1, max_x / 2 - 4, "Score: %d", lData->score);
			attroff(COLOR_PAIR(3));

			high_score(lData);
			refresh();
			getchar();
			endwin();
			free_list(lData);
			return 0;
		}

		if (lData->head->y_direction)
			usleep(150000);
		else 
			usleep(37500);
	}
}
