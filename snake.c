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

// void border_make(struct winsize size) {
	
// 	int height = size.ws_row - 2;
// 	for (int i = 0; i < height; i++) {
// 		if (i == 0 || i == height - 1) {
// 			// printf("%*s%s", size.ws_col, "*", ""); 
// 			// printf("%s%019s\n", "buffer", "*");
// 			for (int j = 0; j < size.ws_col; j++) {
// 				printf("*");
// 			}
// 			printf("\n");
// 			continue;
// 		}
// 		printf("*%*s", size.ws_col - 1, "*");
// 	}
// }

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

int main() {
	int x, y, max_x, max_y;
	int direction_x = 0, direction_y = 1;
	int next_x, next_y;


	initscr();
	noecho();  // no keyboard input
	curs_set(FALSE);
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(max_y / 2 - 1, max_x  / 2 - 7, "Classic Snake");
	mvprintw(max_y / 2, max_x / 2, ">");
	x = max_x / 2;
	y = max_y / 2;
	refresh();

	getchar();

	while(1) {
		int apple[2];
		// int leading_char;

		clear();
		getmaxyx(stdscr, max_y, max_x);
		char *head = head_of_the_snake(direction_x, direction_y);
		mvprintw(y, x, head);

		// refresh();
		next_x = x + direction_x;
		next_y = y + direction_y;
		if (next_x > max_x || next_x < 0) {
			direction_x *= -1;
		} else if (next_y > max_y || next_y < 0) {
			direction_y *= -1;
		}
		x += direction_x;
		y += direction_y;
		usleep(150000);
		random_int(apple, max_x, max_y);
		mvprintw(apple[1], apple[0], "%d,%d", apple[0], apple[1]);
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
*/
