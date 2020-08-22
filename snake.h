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
} Node;

typedef struct data {
	struct node *head;
	struct node *tail;
	int score; 
	int x_apple;
	int y_apple;
} Data;

// function declerations
void upper(char str[10]);
void high_score(Data* lData);
int convert_color_input(char color[10]);
void process_color(char snake_c[10], char apple_c[10]);
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


#endif // SNAKE_H_
