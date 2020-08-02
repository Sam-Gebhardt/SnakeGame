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
	struct node *head;
	struct node *tail;
	int x_cord;
	int y_cord;
} Node;

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

// void add_node(struct *Node list, int x_cord, int y_cord) {

// }


const Node *create_snake(void) {

	const Node *snake_body = (Node*)malloc(sizeof(Node*));
	return snake_body;
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
	mvprintw(max_y / 2 - 1, max_x  / 2 - 7, "Classic Snake");
	mvprintw(max_y / 2, max_x / 2, ">");

	x = max_x / 2;
	y = max_y / 2;

	snake = create_snake();
	refresh();
	getchar();
	nodelay(stdscr, true);

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
	

		refresh();
		getmaxyx(stdscr, max_y, max_x);
		char *head = head_of_the_snake(direction_x, direction_y);
		mvprintw(y, x, head);

		if ((x > max_x || x < 0) || (y > max_y || y < 0)) {

			clear();
			mvprintw(max_y / 2, max_x / 2 - 5, "Game Over!");
			refresh();
			getchar();
			endwin();
			return 0;
		}
		
		x += direction_x;
		y += direction_y;

		if (direction_y) {
			usleep(150000);
		} else {
			usleep(37500);
		}
		if (! uneaten_apple) {
			uneaten_apple = 1;
			random_int(apple, max_x, max_y);
			mvprintw(apple[1], apple[0], "@");
			refresh();

		}

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
