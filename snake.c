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
#include "HashSet.h"
#include "logging.h"

// Color ints
#define SNAKE_COLOR 1
#define APPLE_COLOR 2
#define TEXT_COLOR 3
#define BACKGROUND_COLOR 4

// Path to highscore file
#define HIGHSCORE_PATH "/home/sam/Documents/project/snake_game/.highscore.txt"

// Path to log file
#define LOG_PATH "/home/sam/Documents/project/snake_game/logs.txt"

// Logging function
static Log l;


void upper(char str[BUFSIZ]) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] = str[i] - 32;
	}
}

void high_score(Data* lData) {

	FILE *f = NULL;
	int high, x, y, new = 0;
	attron(COLOR_PAIR(TEXT_COLOR));

	//file does not exsist
	if (access(HIGHSCORE_PATH, F_OK == -1)) {
		f = fopen(HIGHSCORE_PATH, "w");
		fprintf(f, "%d", lData->score);
		fclose(f);

	} else {
		f = fopen(HIGHSCORE_PATH, "r+");
		fscanf(f, "%d", &high);
		fclose(f);
		f = NULL; // always null pointers

		if (lData->score > high) {
			f = fopen(HIGHSCORE_PATH, "w+");
			fprintf(f, "%d", lData->score);
			new = 1;
		}

		getmaxyx(stdscr, y, x);
		char *message = (new) ?  "New High Score!: %d" : "High Score: %d";
		int displacement = (new) ? 10 : 7;
		int score = (new) ? lData->score : high;

		mvprintw((y / 2) + 2, (x / 2) - displacement, message, score);
		if (f != NULL)
			fclose(f);
	}

	attroff(COLOR_PAIR(TEXT_COLOR));
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
	else
		return 7;

}

void process_color(char snake_c[BUFSIZ], char apple_c[BUFSIZ], char background_c[BUFSIZ]) {
	char possible_colors[46] = "black|red|green|yellow|blue|magenta|cyan|white";
	int snake_convert, apple_convert, back_convert;

	if (strstr(possible_colors, snake_c) == NULL ||  // make sure color in put is valid
	    strstr(possible_colors, apple_c) == NULL ||
		strstr(possible_colors, background_c) == NULL) {

		clear();
		attron(COLOR_PAIR(SNAKE_COLOR));
		mvprintw(0, 1, "Invalid color: Using default");

		refresh();
		attroff(COLOR_PAIR(SNAKE_COLOR));

		sleep(1);
		return;
	}

	upper(snake_c);
	upper(apple_c);
	upper(background_c);

	snake_convert = convert_color_input(snake_c);
	apple_convert = convert_color_input(apple_c);
	back_convert = convert_color_input(background_c);

	if (snake_convert == back_convert || apple_convert == back_convert) {
		attron(COLOR_PAIR(SNAKE_COLOR));
		mvprintw(0, 1, "Background color must be unique: Using default");
		attroff(COLOR_PAIR(SNAKE_COLOR));
		refresh();
		sleep(1);
		return;
	}

	printf("%d and %d", snake_convert, apple_convert);
	init_pair(SNAKE_COLOR, snake_convert, back_convert);
	init_pair(APPLE_COLOR, apple_convert, back_convert);
	init_pair(BACKGROUND_COLOR, back_convert, back_convert);
	// overwrites orginal pairs in main() based on input
}

void custom_color(void) {

	int max_y;
	char snake_c[BUFSIZ], apple_c[BUFSIZ], back_c[BUFSIZ];
	char colors[54] = "\nblack \nred \ngreen \nyellow \nblue \nmagenta \ncyan \nwhite";

	max_y = getmaxy(stdscr);
	clear();
	attron(COLOR_PAIR(TEXT_COLOR));
	mvprintw(max_y / 2, 0, "Pick a color(for snake): %s", colors);
	getstr(snake_c);

	mvprintw(max_y / 2, 0, "Pick a color(for apple): %s", colors);
	getstr(apple_c);

	mvprintw(max_y / 2, 0, "Pick a color(for background) must be unique: %s", colors);
	getstr(back_c);

	refresh();
	attroff(COLOR_PAIR(TEXT_COLOR));
	process_color(snake_c, apple_c, back_c);

	getchar();
}


int custom_speed(void) {
	// allow for faster or slower speed
	int max_x, max_y, speed;
	char message[37] = "Enter custom speed(default is 3.6s):";
	char speed_str[10];

	clear();
	getmaxyx(stdscr, max_y, max_x);
	attron(COLOR_PAIR(TEXT_COLOR));
	mvprintw(max_y / 2, max_x / 2 - 40, "%s", message);
	attroff(COLOR_PAIR(TEXT_COLOR));
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

void cleanup(Data* lData, Set available) {
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	clear();

	attron(COLOR_PAIR(TEXT_COLOR));
	mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
	mvprintw(max_y / 2 + 1, max_x / 2 - 4, "Score: %d", lData->score);
	attroff(COLOR_PAIR(TEXT_COLOR));

	high_score(lData);
	free_list(lData);

	set_destroy(available);
	l.logging_destroy(l);

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


void gen_apple(Data* lData, Set available) {

	int* vals = set_random(available);

	lData->x_apple = (vals[0]);
	lData->y_apple = (vals[1]);

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


void move_snake(Data* lData, Set available) {

	clear();
	char *head = head_of_the_snake(lData);
	Node* current = lData->head;
	int x = lData->head->x_cord, y = lData->head->y_cord;
	int max_x;

	max_x = getmaxx(stdscr);

	current->x_cord += current->x_direction;
	current->y_cord += current->y_direction;

	attron(COLOR_PAIR(APPLE_COLOR));
	mvprintw(lData->y_apple, lData->x_apple, "@");
	attroff(COLOR_PAIR(APPLE_COLOR));

	attron(COLOR_PAIR(TEXT_COLOR));
	mvprintw(0, max_x - 3, "%d", lData->score);
	attroff(COLOR_PAIR(TEXT_COLOR));

	attron(COLOR_PAIR(SNAKE_COLOR));
	mvprintw(current->y_cord, current->x_cord, head);

	// Remove new head pos from aval
	set_remove(available, current->x_cord, current->y_cord);

	if (current->x_cord == lData->x_apple && current->y_cord == lData->y_apple) {
		grow_snake(lData);
		set_remove(available, lData->tail->x_cord, lData->tail->y_cord);
		gen_apple(lData, available);
	}

	if (lData->score > 1) {

		// Add old tail back into aval pos
		set_add(available, lData->tail->x_cord, lData->tail->y_cord);

		Node* tmp = lData->tail;

		// Removing old tail
		lData->tail = lData->tail->prev;
		lData->tail->next = NULL;

		// Add old tail to new position
		tmp->prev = lData->head;
		tmp->next = lData->head->next;
		tmp->next->prev = tmp;
		lData->head->next = tmp;

		// position is same as old pos of head
		tmp->x_cord = x;
		tmp->y_cord = y;

		tmp->x_direction = lData->head->x_direction;
		tmp->y_direction = lData->head->y_direction;

	} else if (lData->score == 1) {
		// Add old head pos back into aval
		set_add(available, lData->tail->x_cord, lData->tail->y_cord);

		lData->tail->x_cord = x;
		lData->tail->y_cord = y;
	} else {
		set_add(available, lData->head->x_cord, lData->head->y_cord);
	}

	current = lData->head->next;

	while(current != NULL) {
		mvprintw(current->y_cord, current->x_cord, "*");
		current = current->next;
	}

	refresh();
	attroff(COLOR_PAIR(SNAKE_COLOR));
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
		usleep(time);
	}
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

}

int collion(Data* lData, int past_x, int past_y) {
	// check to see if the move is valid
	int max_x, max_y;
	Node* head = lData->head;
	Node* current = lData->head->next;

	// get again incase window was resized
	getmaxyx(stdscr, max_y, max_x);

	while (current != NULL) {

		if (current == lData->head->next) {
			current = current->next;
			continue;
		}

		if (current->x_cord == head->x_cord && current->y_cord == head->y_cord)
			return 1;
		current = current->next;
	}

	// check if the snake has gone out of bounds or eaten itself
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
	init_pair(SNAKE_COLOR, COLOR_GREEN, COLOR_BLACK);
	init_pair(APPLE_COLOR, COLOR_RED, COLOR_BLACK);
	init_pair(TEXT_COLOR, COLOR_BLUE, COLOR_BLACK);
	init_pair(BACKGROUND_COLOR, COLOR_BLACK, COLOR_BLACK);

	attron(COLOR_PAIR(TEXT_COLOR));  //toggle color on
	mvprintw(max_y / 2 - 1, max_x / 2 - 7, "Classic Snake");
	mvprintw(max_y / 2, max_x / 2, ">");
	mvprintw(max_y / 2 + 1, max_x / 2 - 12, "Press 's' for settings!");
	attroff(COLOR_PAIR(TEXT_COLOR));  //toggle color off

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

	head->x_cord = max_x / 2;
	head->y_cord = max_y / 2;

	head->prev = head;
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

	wbkgd(stdscr, COLOR_PAIR(BACKGROUND_COLOR));
	nodelay(stdscr, true);

	// Init logging
	logging_create(&l, LOG_PATH);

	sprintf(l.log_str, "Max x: %d, Max y : %d\n", max_x, max_y);
	l.logging(l);

	// Make the snake
	Data* lData = create_snake(max_x, max_y);

	// Create set of open spaces for apple to generate
	Set available = set_create(max_x * max_y);

	// Fill it with open spots
	for (int i = 0; i < max_x; i++) {
		for (int j = 0; j < max_y; j++) {
			set_add(available, i, j);
		}
	}

	sprintf(l.log_str, "Starting...Set size: %d, total: %d\n", *available.size, max_x * max_y);
	l.logging(l);

	// Remove the starting pos
	set_remove(available, lData->head->x_cord, lData->head->y_cord);

	sprintf(l.log_str, "Set size: %d, snake size: %d\n", *available.size, lData->score);
	l.logging(l);

	gen_apple(lData, available);
	mvprintw(lData->y_apple, lData->x_apple, "@");

	while(1) {

		int move = getch();
		int past_x = lData->head->x_direction, past_y = lData->head->y_direction;

		int change = get_move(lData, move);

		update_direction(lData);

		move_snake(lData, available);

		// change == -1 is when the user presses "esc"
		if (collion(lData, past_x, past_y) || change == -1) {
			cleanup(lData, available);
			return 0;
		}
		snake_sleep(lData, max_x, max_y, speed);

		sprintf(l.log_str, "Set size: %d, snake size: %d\n", *available.size, lData->score);
		l.logging(l);
	}
}

