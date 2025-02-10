#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void startGame();

int main(int argc, char* argv) {
	startGame();
	int i = 1;
	while (true) {
		char response;
		printf("Do you want to play again? y/n: ");
		scanf_s(" %c", &response, 1);
		if (response == 'y') {
			startGame();
			i++;
		}
		else {
			char* str;
			str = i > 1 ? "games" : "game";
			printf("Thanks for playing ;)\n");
			printf("You played %d %s", i, str);
			break;
		}
	}
	return 0;
}