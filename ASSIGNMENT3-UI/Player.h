#pragma once

#include <vector>

#include "Card.h"

using namespace std;

class Player
{
public:
	bool dealerTurn = false;
	bool splitting = false;
	bool switchHand = false;
	
	Player();
	~Player();

	void addCard(Card*);

	Card* getCard(int);
	void removeCard();
	int getCardValue(int);
	int getTotal();
	int getCardCount();
	

private:
	vector<Card*> hand;

	int total;
	int cardValue;
};