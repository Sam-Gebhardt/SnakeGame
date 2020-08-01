/*
Author: Sam Gebhardt
Basic terminal snake game
*/

#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>

void border(struct winsize size) {
	
	int height = size.ws_row - 2;
	for (int i = 0; i < height; i++) {
		if (i == 0 || i == height - 1) {
			// printf("%*s%s", size.ws_col, "*", ""); 
			// printf("%s%019s\n", "buffer", "*");
			for (int j = 0; j < size.ws_col; j++) {
				printf("*");
			}
			printf("\n");
			continue;
		}
		printf("*%*s", size.ws_col - 1, "*");
	}
}

int main() {

	struct winsize window;
	ioctl(0, TIOCGWINSZ, &window);
	
	// printf("length: %d\n", window.ws_row);
	// printf("height: %d\n", window.ws_col);
	// fflush(stdout);
	system("clear");
	int center = (window.ws_col / 2) - 10;
	printf("%*sClassic snake game.\n", center, "");
	border(window);
	printf("Press s to start or q to quit: ");
	int key = getchar();
	if (key == 'q') {
		system("clear");
		return 0;
	}
	

	return 0;
}