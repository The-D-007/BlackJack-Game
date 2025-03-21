#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define DECK_SIZE 52
#define USERNAME_LIMIT 21
struct Card {
	char number;
	char const* suit;
	bool faceUp;
	bool takenByPlayer;
};

struct Player {
	char* username;
	/*unsigned*/ int cash;
	struct Card  cards[5];
	unsigned int cardsSize;
	unsigned int cardTotal;
	unsigned int cardTotalStart;
	unsigned int aceCount;
	unsigned int bet;
	bool turnFaceUp;
	void (*Logic)(struct Player* player, struct Deck* deck);
};

struct Deck {
	struct Card allCards[DECK_SIZE];
};

typedef struct Card Card;
typedef struct Player Player;
typedef struct Deck Deck;



void printFormating() {
	printf("\n-------------------\tKeep Enjoying\t-----------------\n\n");
}


void shuffleDeck(Card cards[DECK_SIZE]) {
	for (int i = DECK_SIZE - 1; i >= 0; i--) {
		int index = rand() % (i + 1);
		Card temp = cards[i];
		cards[i] = cards[index];
		cards[index] = temp;
	}
}

void askDetailsFromInput(char* s, unsigned int* returnValue) {
	
	printf("%s: ", s);
	int scanSucceed = scanf_s(" %u", returnValue);
	scanf_s("%*[^\n]");
	while (!scanSucceed) {
		scanf_s("%*[^\n]");
		printf("\n%s: ", s);
		scanSucceed = scanf_s(" %u", returnValue);
		scanf_s("%*[^\n]");
	}
}

void getCards(Deck* deck, Player* player) {
	int attempts = 0;
	int randomValue = rand() % DECK_SIZE;
	while (deck->allCards[randomValue].faceUp == true) {
		randomValue = rand() % DECK_SIZE;
		attempts++;
		if (attempts > 52) {
			shuffleDeck(deck->allCards);
			for (unsigned int i = 0; i < DECK_SIZE; i++) {
				if (!deck->allCards[i].takenByPlayer) {
					deck->allCards[i].faceUp = false;
				}
			}
			attempts = 0;
		}
	}
	player->cards[player->cardsSize] = deck->allCards[randomValue];
	player->cards[player->cardsSize].takenByPlayer = true;
	if (player->username == "Dealer" && player->cardsSize == 1) {
		player->cardsSize += 1;
		return;
	}
	player->cards[player->cardsSize].faceUp = true;
	player->cardsSize += 1;
}

void getTotal(Player* player) {
	unsigned int totalCalStartIndex = player->cardTotalStart;
	bool hiddenToo = false;
	if (player->username == "Dealer" && player->turnFaceUp) {
		hiddenToo = true;
	}
	unsigned int aceCount = player->aceCount;
	unsigned int total = player->cardTotal;
	for (unsigned int i = player->cardTotalStart; i < player->cardsSize; ++i) {
		if (hiddenToo) {
			player->cards[i].faceUp = true;
		}
		if (player->cards[i].faceUp) {
			char val = player->cards[i].number;
			if (val == 'A') {
				aceCount++;
				total += 11;
			}
			else if (val == 'T' || val == 'J' || val == 'Q' || val == 'K') {
				total += 10;
			}
			else {
				total += val - '0';
			}
			while (total > 21 && aceCount > 0) {
				total -= 10;
				aceCount--;
			}
			totalCalStartIndex++;
		}
	}
	player->aceCount = aceCount;
	player->cardTotal = total;
	player->cardTotalStart = totalCalStartIndex;
}

void prettyPrintCards(Player* player) {
	printf("%s cards are: ", player->username);
	for (unsigned int i = 0; i < player->cardsSize; i++) {
		printf("%c ", player->cards[i].faceUp == true ? player->cards[i].number : '?');
	}
	printf(" (Total: %d)\n", player->cardTotal);
}


void smartBot(struct Player* player, struct Deck* deck) {
	if (player->cardTotal >= 17) {
		return;
	}
	while (true) {
		int count = 0;
		for (unsigned int i = 0; i < DECK_SIZE; i++) {
			Card card = deck->allCards[i];
			if (card.faceUp) {
				char cardNum = card.number;
				if (cardNum == '2' || cardNum == '3' || cardNum == '7') {
					count++;
				}
				else if (cardNum == '4' || cardNum == '5' || cardNum == '6') {
					count += 2;
				}
				else if (cardNum == '9') {
					count--;
				}
				else if (cardNum == 'T' || cardNum == 'J' || cardNum == 'Q' || cardNum == 'K') {
					count -= 2;
				}
			}
		}
		if (count < 7) {
			getCards(deck, player);
			getTotal(player);
			prettyPrintCards(player);
			if (player->cardTotal >= 17) {
				return;
			}
		}
		else {
			return;
		}
	}
	return;
}

void dumbBot(struct Player* player, struct Deck* deck) {
	while (true) {
		int randomChoice = rand() % 3;
		bool choose = randomChoice == 2 ? true : false;
		if (choose) {
			printf("Bot chooses to HIT\n");
			getCards(deck, player);
			getTotal(player);
			prettyPrintCards(player);
			if (player->cardTotal >= 21) {
				return;
			}
		}
		else {
			printf("\nBot chooses to STAND\n");
			return;
		}
	}
}

void human(Player* player, Deck* deck) {
	while (true) {
		unsigned int response;
		char str[80];
		strcpy_s(str, 80, player->username);
		strcat_s(str, 80, " want to STAND(1) or HIT(2)");
		askDetailsFromInput(str, &response);
		if (response == 1) {
			return;
		}
		else if (response == 2) {
			getCards(deck, player);
			getTotal(player);
			prettyPrintCards(player);
			if (player->cardTotal >= 21) {
				return;
			}
		}
	}
}

Player* initNewPlayer(char* username) {
	Player* newPlayer = malloc(sizeof(*newPlayer));
	if (!newPlayer) {
		return NULL;
	}
	newPlayer->username = username;

	newPlayer->aceCount = 0;
	newPlayer->bet = 0;
	newPlayer->cash = 1000;
	newPlayer->cardTotal = 0;
	newPlayer->cardsSize = 0;
	newPlayer->cardTotalStart = 0;

	if (username != "Dealer") {
		newPlayer->turnFaceUp = true;
	}
	else {
		newPlayer->turnFaceUp = false;
	}

	if (username == "Bot") {
		newPlayer->Logic = dumbBot;
	}
	else if (username == "AdvBot") {
		newPlayer->Logic = smartBot;
	}
	else {
		newPlayer->Logic = human;
	}
	return newPlayer;
}



void initDeck(Card cards[DECK_SIZE]) {
	char const* suits[] = { "Club", "Diamond", "Heart", "Spade" };
	for (int i = 0; i < DECK_SIZE; i++) {
		cards[i].faceUp = false;
		cards[i].takenByPlayer = false;
		cards[i].suit = suits[i / 13];
		int value = (i % 13) + 1;
		if (value == 1) {
			cards[i].number = 'A';
		}
		else if (value == 10) {
			cards[i].number = 'T';
		}
		else if (value > 10) {
			int faceValue = value - 11;
			if (faceValue == 0) {
				cards[i].number = 'J';
			}
			else if (faceValue == 1) {
				cards[i].number = 'Q';
			}
			else {
				cards[i].number = 'K';
			}
		}
		else {
			cards[i].number = '0' + value;
		}
	}
	shuffleDeck(cards);
}

Deck* createNewDeck() {
	Deck* newDeck = malloc(sizeof(*newDeck));
	if (!newDeck) {
		return NULL;
	}
	initDeck(newDeck->allCards);
	return newDeck;
}

void gameWinConditions(Player* player, Player* dealer) {
	unsigned int userTotal = player->cardTotal;
	unsigned int dealersTotal = dealer->cardTotal;
	unsigned int userBet = player->bet;
	if (userTotal == dealersTotal) {
		printf("No one wins.\n");
	}
	else if (userTotal > 21) {
		printf("Dealer wins from %s!!\n", player->username);
		player->cash -= userBet;
	}
	else if (dealersTotal > 21) {
		userBet = userBet * 3 / 2;
		printf("%s wins!! amount added %u\n", player->username, userBet);
		player->cash += userBet;

	}
	else if (dealersTotal > userTotal) {
		printf("Dealer wins from %s!!\n", player->username);
		player->cash -= userBet;
	}
	else {
		userBet = userBet * 3 / 2;
		printf("%s wins!! amount added %u\n", player->username, userBet);
		player->cash += userBet;
	}
}

void getUserBet(Player* player) {
	if (player->username == "Bot" || player->username == "AdvBot") {
		player->bet = 100;
		return;
	}
	unsigned int bet;
	char str[80];
	strcpy_s(str, 80,player->username);
	strcat_s(str, 80," your bet, whole number only");
	askDetailsFromInput(str, &bet);
	while (bet > player->cash || bet <= 0) {
		printf("\n**You must keep the above 0 and below your balance %d**\n", player->cash);
		askDetailsFromInput(str, &bet);
	}
	player->bet = bet;
}

void returnCards(Player* player) {
	for (unsigned int i = 0; i < player->cardsSize; i++) {
		player->cards[i].takenByPlayer = false;
		
	}
	player->cardsSize = 0;
	player->aceCount = 0;
	player->cardTotal = 0;
	player->cardTotalStart = 0;
	
}

unsigned int gameUsers() {
	unsigned int userInput;
	char* str = "Game options:\n\t1. Play with dealer only.\n\t2. Play with dealer and two bots.\n\t3. Play with dealer and your two friends.\nEnter the number of the option";
	askDetailsFromInput(str, &userInput);
	while (userInput < 1 || userInput > 3) {
		printf("\n");
		askDetailsFromInput(str, &userInput);
	}
	return userInput;
}

void playGame(Deck* deck, Player* players[], int numbersOfPlayers, Player* dealer) {
	for (int i = 0; i < numbersOfPlayers; i++) {
		getUserBet(players[i]);
	}
	printFormating();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < numbersOfPlayers; j++) {
			getCards(deck, players[j]);
		}
		getCards(deck, dealer);
	}
	for (int i = 0; i < numbersOfPlayers; i++) {
		getTotal(players[i]);
	}
	getTotal(dealer);

	for (int i = 0; i < numbersOfPlayers; i++) {
		prettyPrintCards(players[i]);
	}
	prettyPrintCards(dealer);

	printFormating();

	for (int i = 0; i < numbersOfPlayers; i++) {

		players[i]->Logic(players[i], deck);
	}

	printFormating();
	dealer->turnFaceUp = true;
	getTotal(dealer);
	prettyPrintCards(dealer);
	bool seen[3];
	seen[0] = false;
	seen[1] = false;
	seen[2] = false;

	while (dealer->cardTotal < 17 || (dealer->cardTotal == 17 && dealer->aceCount != 0)) {
		for (int i = 0; i < numbersOfPlayers; i++) {
			if (players[i]->cardTotal < dealer->cardTotal && !seen[i]) {
				seen[i] = true;
			}
		}
		if ((seen[0] && numbersOfPlayers == 1) || (seen[0] && seen[1] && seen[2])) {
			break;
		}
		getCards(deck, dealer);
		getTotal(dealer);
		prettyPrintCards(dealer);
	}

	for (int i = 0; i < numbersOfPlayers; i++) {
		gameWinConditions(players[i], dealer);
	}

	for (int i = 0; i < numbersOfPlayers; i++) {
		returnCards(players[i]);
	}
	returnCards(dealer);
}

void destroyPlayer(Player** pPlayer) {
	if (*pPlayer) {
		free(*pPlayer);
		*pPlayer = NULL;
	}
}

void destroyDeck(Deck** deck) {
	if (*deck) {
		free(*deck);
		*deck = NULL;
	}
}

void takeUsername(char username[USERNAME_LIMIT]) {
	printf("\n");
	printf("Enter the username (username can't be 'Bot', 'AdvBot', 'Dealer' or more than 20 chars): ");
	int convert = scanf_s("%s", username, USERNAME_LIMIT);
	username[USERNAME_LIMIT - 1] = '\0';

	while ((strcmp(username, "Bot") == 0) || (strcmp(username, "AdvBot") == 0) || (strcmp(username, "Dealer") == 0)) {
		printf("\nEnter the username (username can't be 'Bot', 'AdvBot', 'Dealer' or more than 20 chars): ");
		scanf_s("%s", username, USERNAME_LIMIT);
		username[USERNAME_LIMIT - 1] = '\0';
		printf("%s", username);
	}
}

void clearValues(Player* player) {
	player->aceCount = 0;
	player->cards;
}

void startGame() {
	srand(time(0));
	Deck* deck = createNewDeck();
	Player* dealer = initNewPlayer("Dealer");
	unsigned int option = gameUsers();
	Player* players[3];
	bool playGameAgain = true;
	
	switch (option) {
	case 1: {
		char username[USERNAME_LIMIT];
		takeUsername(username);
		Player* player1 = initNewPlayer(username);
		players[0] = player1;
		printFormating();
		while (playGameAgain) {
			playGame(deck, players, 1, dealer);
			unsigned int returnValue;
			askDetailsFromInput("Enter 0 to play again", &returnValue);
			if (returnValue != 0) {
				playGameAgain = false;
			} 
			if (player1->cash <= 0) {
				printf("%s you don't have enough cash(%d), Game Over :(", player1->username, player1->cash);
				playGameAgain = false;
			}
		}
		break;
	}
	case 2: {
		char username[USERNAME_LIMIT];
		takeUsername(username);
		Player* player1 = initNewPlayer(username);
		Player* player2 = initNewPlayer("Bot");
		Player* player3 = initNewPlayer("AdvBot");
		players[0] = player1;
		players[1] = player2;
		players[2] = player3;
		printFormating();
		while (playGameAgain) {
			playGame(deck, players, 3, dealer);
			unsigned int returnValue;
			askDetailsFromInput("Enter 0 to play again", &returnValue);
			if (returnValue != 0) {
				playGameAgain = false;
			}
			if (player1->cash <= 0) {
				printf("%s you don't have enough cash(%d), Game Over :(", player1->username, player1->cash);
				playGameAgain = false;
			}
		}
		break;
	}
	case 3: {
		char username1[USERNAME_LIMIT];
		char username2[USERNAME_LIMIT];
		char username3[USERNAME_LIMIT];
		takeUsername(username1);
		takeUsername(username2);
		takeUsername(username3);
		Player* player1 = initNewPlayer(username1);
		Player* player2 = initNewPlayer(username2);
		Player* player3 = initNewPlayer(username3);
		players[0] = player1;
		players[1] = player2;
		players[2] = player3;
		printFormating();
		playGame(deck, players, 3, dealer);
		break;
	}
	}
	printFormating();
}