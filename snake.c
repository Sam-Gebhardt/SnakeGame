/*
Author: Sam Gebhardt
Basic terminal snake game
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "snake.h"


void upper(char str[BUFSIZ]) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] = str[i] - 32;
	}
}

void high_score(Data* lData) {

	FILE *f = NULL;
	int high, x, y, new = 0;
	attron(COLOR_PAIR(3));

	if (access(".highscore.txt", F_OK == -1)) { //file does not exsist
		f = fopen(".highscore.txt", "w");
		fprintf(f, "%d", lData->score);
		fclose(f);

	} else {									//file does exsist
		f = fopen(".highscore.txt", "r+");
		fscanf(f, "%d", &high);
		fclose(f);
		f = NULL; // always null pointers

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
		if (f != NULL)
			fclose(f);
	}

	attroff(COLOR_PAIR(3));
}

int convert_color_input(char color[BUFSIZ]) {
	// convert str to int rep. of a color defined by curses

	if (strcmp(color, "BLACK") == 0)
		return 0;
	else if (strcmp(color, "RED") == 0)
		return 1;
	else if (strcmp(color, "GREEN") == 0)
		return 2;
	else if (strcmp(color, "YELLOW") == 0)
		return 3;
	else if (strcmp(color, "BLUE") == 0)
		return 4;
	else if (strcmp(color, "MAGENTA") == 0)
		return 5;
	else if (strcmp(color, "CYAN") == 0)
		return 6; 
	else //(strcmp(color, "WHITE")
		return 7;

}

void process_color(char snake_c[BUFSIZ], char apple_c[BUFSIZ]) {
	char possible_colors[BUFSIZ] = "black|red|green|yellow|blue|magenta|cyan|white";
	int snake_convert, apple_convert;

	if (strstr(possible_colors, snake_c) == NULL ||
	    strstr(possible_colors, apple_c) == NULL) {

		clear();
		attron(COLOR_PAIR(1));
		mvprintw(0, 1, "Invalid color: Using default");

		refresh();
		attroff(COLOR_PAIR(1));

		sleep(1);
		return;
	}
	upper(snake_c);
	upper(apple_c);

	snake_convert = convert_color_input(snake_c);
	apple_convert = convert_color_input(apple_c);

	printf("%d and %d", snake_convert, apple_convert);
	init_pair(1, snake_convert, COLOR_BLACK);   // snake
	init_pair(2, apple_convert, COLOR_BLACK);   // apple
}

void custom_color(void) {

	int max_y;
	char snake_c[BUFSIZ], apple_c[BUFSIZ];
	char colors[BUFSIZ] = "\nblack \nred \ngreen \nyellow \nblue \nmagenta \ncyan \nwhite";

	max_y = getmaxy(stdscr);
	clear();
	attron(COLOR_PAIR(3));
	mvprintw(max_y / 2, 0, "Pick a color(for snake): %s", colors);
	getstr(snake_c);

	mvprintw(max_y / 2, 0, "Pick a color(for apple): %s", colors);
	getstr(apple_c);

	refresh();
	attroff(COLOR_PAIR(3));
	process_color(snake_c, apple_c);

	getchar();
}


int custom_speed(void) {
	// allow for faster or slower speed
	int max_x, max_y, speed;
	char message[BUFSIZ] = "Enter custom speed(default is 3.6s):";
	char speed_str[10];

	clear();
	getmaxyx(stdscr, max_y, max_x);
	attron(COLOR_PAIR(3));
	mvprintw(max_y / 2, max_x / 2 - 40, "%s", message);
	attroff(COLOR_PAIR(3));
	refresh();
	getstr(speed_str);

	for (int i = 0; speed_str[i] != '\0'; i++) {
		if (! isdigit(speed_str[i])) {
			return 0;
		}
	}

	speed = atoi(speed_str);
	if (speed <= 0) 
		return 0;
	
	return speed;
}


void settings(void) {
	int input = getchar();

	if (input != 's') {

	}
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

void cleanup(Data* lData) {
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	clear();

	attron(COLOR_PAIR(3));
	mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
	mvprintw(max_y / 2 + 1, max_x / 2 - 4, "Score: %d", lData->score);
	attroff(COLOR_PAIR(3));

	high_score(lData); // fix mem issues
	free_list(lData);

	refresh();
	getchar();
	endwin();
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

int get_move(Data* lData, int move) {
	int change = 0;
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
			return -1;
	}
	return change;
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

void snake_sleep(Data* lData, int max_x, int max_y, int speed) {
	int time;
	speed *= 1000000;

	if (speed == 0) // default case is 3.6 seconds
		speed = 3600000;

	if (lData->head->y_direction) {
		time = speed / max_y;
		usleep(time);
	} else {
		time = speed / max_x;
		usleep(time); //37500
	}
}

void grow_snake(Data* lData) {
	
	Node* new = (Node*)malloc(sizeof(Node));
	new->x_cord = lData->tail->x_cord - lData->tail->x_direction;
	new->y_cord = lData->tail->y_cord - lData->tail->y_direction;

	new->x_direction = lData->tail->x_direction;
	new->y_direction = lData->tail->y_direction;
	new->pivot = 0;

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

int screen_init(int max_x, int max_y) {
	
	noecho();  // no keyboard input
	curs_set(FALSE);
	keypad(stdscr, TRUE);

	if (has_colors() == FALSE) {
		endwin();
		printf("Your terminal doen't support color.\n");
		return 1;
	}

	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK); // snake
	init_pair(2, COLOR_RED, COLOR_BLACK);   // apple
	init_pair(3, COLOR_BLUE, COLOR_BLACK); // text
	init_pair(4, COLOR_BLACK, COLOR_BLACK); // background
	wbkgd(stdscr, COLOR_PAIR(4));


	attron(COLOR_PAIR(3));
	mvprintw(max_y / 2 - 1, max_x / 2 - 7, "Classic Snake");
	mvprintw(max_y / 2, max_x / 2, ">");
	attroff(COLOR_PAIR(3));

	refresh();
	return 0;
}

Data* create_snake(int max_x, int max_y) {
	Node* head = (Node*)malloc(sizeof(Node));
	Data* lData = (Data*)malloc(sizeof(Data));

	lData->head = head;
	lData->tail = head;
	lData->score = 0;

	head->x_direction = 1;
	head->y_direction = 0;
	head->pivot = 0;

	head->x_cord = max_x / 2;
	head->y_cord = max_y / 2;

	head->prev = NULL;
	head->next = NULL;

	return lData;
}

int main() {
	int max_x, max_y, speed = 0;

	initscr();
	getmaxyx(stdscr, max_y, max_x);
	if (screen_init(max_x, max_y)) 
		return 1;

	int input = getchar();
	if (input == 's') {
		custom_color();
		speed = custom_speed();
	} 

	nodelay(stdscr, true);
	Data* lData = create_snake(max_x, max_y);
	gen_apple(lData);
	mvprintw(lData->y_apple, lData->x_apple, "@");

	while(1) {
		
		int move = getch();
		int past_x = lData->head->x_direction, past_y = lData->head->y_direction;

		int change = get_move(lData, move);

		if (change == -1) {
			cleanup(lData);
			return 0;
		}
		if (change)
			update_direction(lData);
		
		move_snake(lData);

		if (collion(lData, past_x, past_y)) {

			cleanup(lData);
			return 0;
		}
	snake_sleep(lData, max_x, max_y, speed);
	}
}

/* TODO:

**Readability** 
Clean up main before loop

allow for custom speed and custom color

*/
