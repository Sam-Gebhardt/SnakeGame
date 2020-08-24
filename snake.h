#ifndef SNAKE_H_
#define SNAKE_H_

typedef struct node {
	struct node *next;
	struct node *prev;
	int x_cord;
	int y_cord;
	int x_direction;
	int y_direction;
	int pivot; // change in direction (causes "snake" effect)
    /*
    A linked list node that serves as the snake. Holds the current location on the
    screen in x/y_cord, the current direction the node is moving in x/y_direction, 
    and if the node is a pivot.
    */
} Node;

typedef struct data {
	struct node *head;
	struct node *tail;
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
    Turns str into all upper case
    */


void high_score(Data* lData);
    /*
    Checks if the final score is higher than the highscore. If there is no
    high score a file highscore.txt is created.
    */


int convert_color_input(char color[10]);
    /*
    Converts the str of a color into its integar representation in curses.
    ie "BLACK" -> 0
    */


void process_color(char snake_c[10], char apple_c[10]);
    /*
    Check if the inputed color is valid and creates a color pair if it is.
    */


void custom_color(void);
    /*
    Gets user input for setting custom colors or defaults to normal color pairs.
    */


void free_list(Data* lData);
    /*
    Calls free on each node in the linked list and on lData struc
    */


void cleanup(Data* lData);
	/*
    Does everything required to terminate the program. Calls free_list(), displays the
    score and ends the window.
	*/


char *head_of_the_snake(Data* lData);
	/*
    Returns the correct char for the head based on the current direction.
	*/


void gen_apple(Data* lData);
	/*
    Randomly picks a new place on the screen to spawn an apple.
	*/


int backwards(Data* lData, int past_x, int past_y);
	/*
    Checks to see if the snake made an illegal backwards move.
	*/


void update_direction(Data* lData);
	/*
	*/


int get_move(Data* lData, int move);
	/*
    Updates the direction of the head of the snake based on user input.
	*/


void move_snake(Data* lData);
	/*
    Updates the snake's position on the screen.
	*/


void grow_snake(Data* lData);
	/*
    Increases the size of the snake after an apple has been "eaten". 
	*/


int collison(Data* lData, int past_x, int past_y);
	/*
    Detects if the snake has attempted to eat itself.
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