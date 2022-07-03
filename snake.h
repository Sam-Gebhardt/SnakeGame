#ifndef SNAKE_H_
#define SNAKE_H_

#include "HashSet.h"

typedef struct node {
	struct node *next;
	struct node *prev;
	int x_cord;
	int y_cord;
    /*
    A linked list node that serves as the snake. Holds the current location on the
    screen in x/y_cords and the current direction the node is moving in x/y_direction.
    */
} Node;

typedef struct data {
	struct node *head;
	struct node *tail;
    int x_direction;
	int y_direction;
	int score;
	int x_apple;
	int y_apple;
    /*
    Holds general data for the linked list. Head/tail, the score (length) and cords of
    the current apple.
    */
} Data;

// function declerations
void upper(char str[10]);
    /*
    Turns str into all upper case. Used to convert user input into
    a standard form.
    */


void high_score(Data* lData);
    /*
    Checks if the final score is higher than the highscore, if it is prints to the
    screen that there is a new high score. If there is no high score a file highscore.txt
    is created.
    */


int convert_color_input(char color[10]);
    /*
    Converts the str of a color into its integar representation in ncurses.
    ie "BLACK" -> 0
    */


void process_color(char snake_c[10], char apple_c[10], char back_c[10]);
    /*
    Check if the inputed color is valid and creates a color pair if it is.
    */


void custom_color(void);
    /*
    Gets user input for setting custom colors or defaults to normal color pairs.
    */


int custom_speed(void);
    /*
    Changes the speed based on user input.
    */


void free_list(Data* lData);
    /*
    Calls free on each node in the linked list and on lData struc
    */


void cleanup(Data* lData, Set available);
	/*
    Does everything required to terminate the program. Calls free_list(), displays the
    score and ends the window.
	*/


char *head_of_the_snake(Data* lData);
	/*
    Returns the correct char for the head based on the current direction.
    ie if up_arrow is clicked, then the character is '^'
	*/


void gen_apple(Data* lData, Set available);
	/*
    Randomly picks a new place on the screen to spawn an apple.
	*/


int backwards(Data* lData, int past_x, int past_y);
	/*
    Checks to see if the snake made an illegal backwards move.
	*/


void update_direction(Data* lData);
	/*
    Sets the head of the snake to the direction specified by user input.
	*/


int get_move(Data* lData, int move);
	/*
    Reads user input and updates the direction of movement accordingly.
	*/


void move_snake(Data* lData, Set availble);
	/*
    Updates the snake's position on the screen.
	*/


void snake_sleep(Data* lData, int max_x, int max_y, int speed);
    /*
    Create a delay so the snake moves at a reasonable speed
    */


void grow_snake(Data* lData);
	/*
    Increases the size of the snake after an apple has been "eaten".
	*/


int collison(Data* lData);
	/*
    Detects if the snake has attempted to eat itself, or go out of bounds.
	*/


int screen_init(int max_x, int max_y);
	/*
    Starts the screen and gets custom colors from user input.
	*/


Data* create_snake(int max_x, int max_y);
	/*
    Creats first node in the linked list and returns a Data* with a pointer
    to the head and tail nodes.
	*/


// function declerations
#endif // SNAKE_H_
