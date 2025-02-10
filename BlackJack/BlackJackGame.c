#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct black_jack_game_deck {
	char number;
	bool faceUp;
};

typedef struct black_jack_game_deck deck;

void shuffle_deck(deck arr[52], int deckLen) {
	for (int i = deckLen - 1; i >= 0; i--) {
		int index = rand() % (i + 1);
		deck temp = arr[i];
		arr[i] = arr[index];
		arr[index] = temp;
	}
}

void init_black_jack_game_deck(deck arr[52], int deckLen) {
	for (int i = 0; i < deckLen; i++) {
		arr[i].faceUp = false;
		int value = (i % 13) + 1;
		if(value == 1) {
			arr[i].number = 'A';
		}else if (value == 10) {
			arr[i].number = 'T';
		}
		else if (value > 10) {
			int faceValue = value - 11;
			if (faceValue == 0) {
				arr[i].number = 'J';
			}
			else if (faceValue == 1) {
				arr[i].number = 'Q';
			}
			else {
				arr[i].number = 'K';
			}
		}
		else {
			arr[i].number ='0' + value;
		}
	}

	shuffle_deck(arr, deckLen);
}

void get_total(char val, int * total, int * aceCount) {
	if (val == 'A') {
		(*aceCount)++;
		*total += 11;
	}
	else if (val == 'T' || val == 'J' || val == 'Q' || val == 'K') {
		*total += 10;
	} else{
		*total += val - '0';
	}
	while (*total > 21 && *aceCount > 0) {
		*total -= 10;
		(*aceCount)--;
	}
}

void pretty_print_cards(char* name, char arr[], int len, int total) {
	printf("%s cards are: ", name);
	for (int i = 0; i < len; i++) {
		printf("%c ", arr[i]);
	}
	printf(" (Total: %d)\n", total);
}

void get_cards(deck arr[], int deckLen, char cardOf[], int i) {
	int attempts = 0;
	int random_value = rand() % deckLen;
	while (arr[random_value].faceUp == true) {
		random_value = rand() % deckLen;
		attempts++;
		if (attempts > 52) {
			shuffle_deck(arr, deckLen);
			for (int j = 0; j < deckLen; j++) {
				arr[j].faceUp = false;
			}
			attempts = 0;
		}
	}
	cardOf[i] = arr[random_value].number;
	arr[random_value].faceUp = true;
}

void game_win_conditions(int userTotal, int dealersTotal, unsigned int* pAmount, unsigned int bet) {
	if (userTotal == dealersTotal) {
		printf("No one wins.\n");
	} else if (dealersTotal > 21) {
		printf("You wins!! amount added %u\n", bet);
		*pAmount += bet * 3 / 2;
	}
	else if (dealersTotal > userTotal) {
		printf("Dealer wins!!\n");
		*pAmount -= bet;
	}
	else {
		printf("You wins!! amount added %u\n", bet);
		*pAmount += bet * 3 / 2;
	}
}

bool initial_game_check_condition(int userTotal, int dealersIntialTotal, unsigned int * pAmount, unsigned int bet ) {
	if (userTotal == 21 && dealersIntialTotal == 21) {
		printf("Push! No one wins.\n");
		return true;
	}
	else if (userTotal == 21) {
		printf("You wins!! amount added %u\n", bet);
		*pAmount += bet * 3 / 2;
		return true;
	}
	else if (dealersIntialTotal == 21) {
		printf("Dealer has Blackjack! You lose.\n");
		*pAmount -= bet;
		return true;
	}
	else {
		return false;
	}

}

void playGame(unsigned int * pAmount, deck arr[], int deckLen) {
	unsigned int bet;
	printf("Enter your bet: ");
	scanf_s(" %u", &bet); 
	while (bet > *pAmount || bet <= 0) {
		printf("Enter your bet again: ");
		scanf_s(" %u", &bet);
	}
	char userCards[21];
	int userTotal = 0;
	int userAceCount = 0;
	for (int i = 0; i < 2; i++) {
		get_cards(arr, deckLen, userCards, i);
		get_total(userCards[i], &userTotal, &userAceCount);
	}

	char dealersCards[21];
	int dealersTotal = 0;
	int dealersAceCount = 0;
	for (int i = 0; i < 2; i++) {
		get_cards(arr, deckLen, dealersCards, i);
	}
	get_total(dealersCards[0], &dealersTotal, &dealersAceCount);

	int dealersInitialTotal = 0;
	get_total(dealersCards[0], &dealersInitialTotal, &dealersAceCount);
	get_total(dealersCards[1], &dealersInitialTotal, &dealersAceCount);
	if (initial_game_check_condition(userTotal, dealersInitialTotal, pAmount, bet)) {
		return;
	}

	pretty_print_cards("Your Cards", userCards, 2, userTotal);	
	printf("Dealers Cards: %c ? (Total: %d)\n", dealersCards[0], dealersTotal);

	
	int userIndex = 2;
	int dealersIndex = 2;
	bool breaked = false;
	while (true) {
		if (userIndex >= 20 || dealersIndex >= 20) { breaked = true;  break; }
		char response;
		printf("\nYou want to STAND(s) or HIT(h)? ");
		scanf_s(" %c", &response, 1);
		if (response == 'h') {
			get_cards(arr, deckLen, userCards, userIndex);
			get_total(userCards[userIndex], &userTotal, &userAceCount);
			userIndex++;
			pretty_print_cards("Your Cards", userCards, userIndex, userTotal);
			
			if (userTotal > 21) {
				get_total(dealersCards[1], &dealersTotal, &dealersAceCount);
				pretty_print_cards("Dealers Cards", dealersCards, 2, dealersTotal);
				printf("Dealer wins!!\n");
				*pAmount -= bet;
				break;
			}
		}
		else if (response == 's') {
			get_total(dealersCards[1], &dealersTotal, &dealersAceCount);
			if (dealersTotal > userTotal) {
				pretty_print_cards("Dealers Cards", dealersCards, 2, dealersTotal);
				printf("Dealer wins!!\n");
				*pAmount -= bet;
				break;
			}
			while (dealersTotal < 17 || (dealersTotal == 17 && dealersAceCount != 0)) {
				get_cards(arr, deckLen, dealersCards, dealersIndex);
				get_total(dealersCards[dealersIndex], &dealersTotal, &dealersAceCount);
				dealersIndex++;
			}
			pretty_print_cards("Dealers Cards", dealersCards, dealersIndex, dealersTotal);
			game_win_conditions(userTotal, dealersTotal, pAmount, bet);
			break;
		} 
		if (breaked) { game_win_conditions(userTotal, dealersTotal, pAmount, bet); };
	}
	
}

void startGame() {
	srand(time(NULL));
	deck arr[52];
	int deckLen = 52;
	init_black_jack_game_deck(arr, deckLen);
	unsigned int amount = 1000;
	while (amount > 0) {
		printf("\nThe amount you have to bet: %u \n\n", amount);
		playGame(&amount, arr, deckLen);
	}
}