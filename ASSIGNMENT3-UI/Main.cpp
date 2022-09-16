#include "Globals.h"

bool init();
void gameLoop();
void eventLoop();
void dealerLoop();
void renderLoop();
void shutdown();

int main(int argv, char** argc)
{
	if (!init())
	{
		atexit(SDL_Quit);

		return -1;
	}

	gameLoop();

	shutdown();

	return 0;
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		cerr << "Error initializing SDL: " << SDL_GetError() << endl;

		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) < 0)
	{
		cerr << "Error initializing SDL_image: " << IMG_GetError() << endl;

		return false;
	}

	window = SDL_CreateWindow("Blackjack", 20, 40, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!window)
	{
		cerr << "Error creating window: " << SDL_GetError() << endl;

		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!renderer)
	{
		cerr << "Error creating renderer: " << SDL_GetError() << endl;

		return false;
	}

	//INITIALIZE SPRITES
	bg = new Sprite("images/BG.png", renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1);
	table = new Sprite("images/table.png", renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1);
	victory = new Sprite("images/Victory-V2.png", renderer, (SCREEN_WIDTH / 2) - 375, (SCREEN_HEIGHT / 2) - 380, 750, 450, 1, 1);
	defeat = new Sprite("images/Defeat-V2.png", renderer, (SCREEN_WIDTH / 2) - 375, (SCREEN_HEIGHT / 2) - 380, 750, 450, 1, 1);
	panel = new Sprite("images/Panel.png", renderer, 0, 0, 2048, 1536, 1, 1);
	

	play = new Button("images/buttons.png", renderer, (SCREEN_WIDTH / 2) - 124, (SCREEN_HEIGHT / 2) - 54, 247, 108, 1, 6);
	quit = new Button("images/Quit.png", renderer, (SCREEN_WIDTH / 2) - 43, (SCREEN_HEIGHT / 2) + 100, 160, 162, 1, 1);

	hit = new Button("images/buttons.png", renderer, 136, 20, 247, 108, 1, 6);
	stand = new Button("images/buttons.png", renderer, 519, 20, 247, 108, 1, 6);
	pass = new Button("images/buttons.png", renderer, 902, 20, 247, 108, 1, 6);
	doubleDown = new Button("images/buttons.png", renderer, 1285, 20, 247, 108, 1, 6);
	split = new Button("images/buttons.png", renderer, 1668, 20, 247, 108, 1, 6);

	if (!bg || !table || !play || !hit || !stand || !pass || !doubleDown || !split || !victory || !defeat || !panel)
	{
		cerr << "Error loading sprites " << IMG_GetError() << endl;

		return false;
	}

	deck = new Deck(renderer);

	player = new Player();
	dealer = new Player();

	play->setCurrentButton(0);
	hit->setCurrentButton(1);
	stand->setCurrentButton(2);
	pass->setCurrentButton(3);
	doubleDown->setCurrentButton(4); 
	split->setCurrentButton(5);
}

void gameLoop()
{
	int totalFrames = 0;
	int startTime = 0;
	int endTime = 0;
	int deltaTime = 0;

	while (!done)
	{
		startTime = SDL_GetTicks();

		++totalFrames;

		eventLoop();
		dealerLoop();

		endTime = SDL_GetTicks();

		deltaTime = endTime - startTime;

		if (deltaTime < TICKSPERFRAME)
		{
			SDL_Delay(TICKSPERFRAME - deltaTime);
		}

		renderLoop();
	}
}

void eventLoop()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.key.repeat == 0)
		{
			switch (event.type)
			{
			case SDL_QUIT:
				done = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (!play->isPressed() && play->isVisible() && event.button.x >= play->getXPos() && event.button.x <= play->getXPos() + play->getWidth() &&
					event.button.y >= play->getYPos() && event.button.y <= play->getYPos() + play->getHeight())
				{
					play->onPress();

					player->addCard(deck->getCard());

					Card* temp = deck->getCard();
					temp->setBack(true);
					dealer->addCard(temp);

					player->addCard(deck->getCard());
					dealer->addCard(deck->getCard());
				}
				else if (!quit->isPressed() && quit->isVisible() && event.button.x >= quit->getXPos() && event.button.x <= quit->getXPos() + quit->getWidth() &&
					event.button.y >= quit->getYPos() && event.button.y <= quit->getYPos() + quit->getHeight())
				{
					quit->onPress();

					done = true;
				}
				else if (!hit->isPressed() && hit->isVisible() && event.button.x >= hit->getXPos() && event.button.x <= hit->getXPos() + hit->getWidth() &&
					event.button.y >= hit->getYPos() && event.button.y <= hit->getYPos() + hit->getHeight())
				{
					hit->onPress();

					if (!player->switchHand)
					{
						player->addCard(deck->getCard());

						if (player->getTotal() == 21)
						{
							win = true;
						}
						else if (player->getTotal() > 21)
						{
							if (player->splitting)
							{
								player->switchHand = true;
							}
							else
							{
								lose = true;
							}
						}
					}
					else if (player->switchHand)
					{
						playerSplit->addCard(deck->getCard());

						if (playerSplit->getTotal() == 21)
						{
							win = true;
						}
						else if (playerSplit->getTotal() > 21 && player->getTotal() < 21)
						{
							player->dealerTurn = true;
						}
						else if (playerSplit->getTotal() > 21 && player->getTotal() > 21)
						{
							lose = true;
						}
					}
				}
				else if (!stand->isPressed() && stand->isVisible() && event.button.x >= stand->getXPos() && event.button.x <= stand->getXPos() + stand->getWidth() &&
					event.button.y >= stand->getYPos() && event.button.y <= stand->getYPos() + stand->getHeight())
				{
					stand->onPress();

					if (!player->splitting || player->switchHand)
					{
						player->dealerTurn = true;
					}
					else if (!player->switchHand)
					{
						player->switchHand = true;
					}
				}
				else if (!pass->isPressed() && pass->isVisible() && event.button.x >= pass->getXPos() && event.button.x <= pass->getXPos() + pass->getWidth() &&
					event.button.y >= pass->getYPos() && event.button.y <= pass->getYPos() + pass->getHeight())
				{
					pass->onPress();

					lose = true;
				}
				else if (!doubleDown->isPressed() && doubleDown->isVisible() && event.button.x >= doubleDown->getXPos() && event.button.x <= doubleDown->getXPos() + doubleDown->getWidth() &&
					event.button.y >= doubleDown->getYPos() && event.button.y <= doubleDown->getYPos() + doubleDown->getHeight())
				{
					doubleDown->onPress();

					if (!player->splitting)
					{
						player->addCard(deck->getCard());
						
						if (player->getTotal() < 21)
						{
							player->dealerTurn = true;
						}
						else if (player->getTotal() == 21)
						{
							win = true;
						}
						else
						{
							lose = true;
						}
					}
					else if (player->splitting && !player->switchHand)
					{
						player->addCard(deck->getCard());

						if (player->getTotal() == 21)
						{
							win = true;
						}
						else
						{
							player->switchHand = true;
						}
					}
					else if (player->switchHand)
					{
						playerSplit->addCard(deck->getCard());
						
						if (playerSplit->getTotal() < 21)
						{
							player->dealerTurn = true;
						}
						else if (playerSplit->getTotal() == 21)
						{
							win = true;
						}
						else
						{
							lose = true;
						}
					}
				}
				else if (!split->isPressed() && split->isVisible() && event.button.x >= split->getXPos() && event.button.x <= split->getXPos() + split->getWidth() &&
					event.button.y >= split->getYPos() && event.button.y <= split->getYPos() + split->getHeight())
				{
					split->onPress();

					player->splitting = true;

					playerSplit = new Player();

					playerSplit->addCard(player->getCard(1));
					player->removeCard();
					player->addCard(deck->getCard());
					playerSplit->addCard(deck->getCard());
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (play->isPressed() && play->isVisible() && event.button.x >= play->getXPos() && event.button.x <= play->getXPos() + play->getWidth() &&
					event.button.y >= play->getYPos() && event.button.y <= play->getYPos() + play->getHeight())
				{
					play->setPressed(false);
				}
				else if (quit->isPressed() && quit->isVisible() && event.button.x >= quit->getXPos() && event.button.x <= quit->getXPos() + quit->getWidth() &&
					event.button.y >= quit->getYPos() && event.button.y <= quit->getYPos() + quit->getHeight())
				{
					quit->setPressed(false);
				}
				else if (hit->isPressed() && hit->isVisible() && event.button.x >= hit->getXPos() && event.button.x <= hit->getXPos() + hit->getWidth() &&
					event.button.y >= hit->getYPos() && event.button.y <= hit->getYPos() + hit->getHeight())
				{
					hit->setPressed(false);
				}
				else if (stand->isPressed() && stand->isVisible() && event.button.x >= stand->getXPos() && event.button.x <= stand->getXPos() + stand->getWidth() &&
					event.button.y >= stand->getYPos() && event.button.y <= stand->getYPos() + stand->getHeight())
				{
					stand->setPressed(false);
				}
				else if (pass->isPressed() && pass->isVisible() && event.button.x >= pass->getXPos() && event.button.x <= pass->getXPos() + pass->getWidth() &&
					event.button.y >= pass->getYPos() && event.button.y <= pass->getYPos() + pass->getHeight())
				{
					pass->setPressed(false);
				}
				else if (doubleDown->isPressed() && doubleDown->isVisible() && event.button.x >= doubleDown->getXPos() && event.button.x <= doubleDown->getXPos() + doubleDown->getWidth() &&
					event.button.y >= doubleDown->getYPos() && event.button.y <= doubleDown->getYPos() + doubleDown->getHeight())
				{
					doubleDown->setPressed(false);
				}
				else if (split->isPressed() && split->isVisible() && event.button.x >= split->getXPos() && event.button.x <= split->getXPos() + split->getWidth() &&
					event.button.y >= split->getYPos() && event.button.y <= split->getYPos() + split->getHeight())
				{
					split->setPressed(false);
				}
				break;
			}
		}
	}
}

void dealerLoop()
{
	if (player->dealerTurn)
	{
		dealer->getCard(0)->setBack(false);

		while (dealer->getTotal() < 17)
		{
			dealer->addCard(deck->getCard());
		}

		if (dealer->getTotal() <= 21)
		{
			if (dealer->getTotal() >= player->getTotal())
			{
				lose = true;
			}
			else
			{
				win = true;
			}
		}
		else
		{
			win = true;
		}
	}
}

void renderLoop()
{
	bg->draw();
	table->draw();

	if (!play->isPressed() && play->isVisible())
	{
		play->draw();
	}
	else if (hit->isPressed() && hit->isVisible())
	{
		hit->setPressed(false);
	}
	else if (stand->isPressed() && stand->isVisible())
	{
		stand->setPressed(false);
	}
	else if (pass->isPressed() && pass->isVisible())
	{
		pass->setVisible(false);
	}
	else if (doubleDown->isPressed() && doubleDown->isVisible())
	{
		
	}
	else if (split->isPressed() && split->isVisible())
	{
		split->setVisible(false);
	}
	else 
	{
		play->setVisible(false);

		hit->draw();		
		stand->draw();

		if (pass->isVisible())
		{
			if (dealer->getCardCount() == 2 && player->getCardCount() == 2)
			{
				if ((dealer->getTotal() - dealer->getCardValue(0)) == 11 || (dealer->getTotal() - dealer->getCardValue(0)) == 1)
				{
					pass->draw();
				}
			}
		}

		if (doubleDown->isVisible())
		{
			if (player->getCardCount() == 2)
			{
				doubleDown->draw();
			}
			else if (playerSplit)
			{
				if (player->getCardCount() == 2)
				{
					doubleDown->draw();
				}
			}
		}

		if (split->isVisible())
		{
			if (player->getCard(0)->getFace() == player->getCard(1)->getFace())
			{
				split->draw();
			}
		}
	}

	if (player->getCardCount() > 0 && player->splitting == false)
	{
		for (int i = 0; i < player->getCardCount(); ++i)
		{
			deck->setXPos(150 + (i * 252));
			deck->setYPos(700);

			deck->drawCard(player->getCard(i));
		}
	}

	if (player->getCardCount() > 0 && player->splitting == true)
	{
		for (int i = 0; i < player->getCardCount(); ++i)
		{
			deck->setXPos(100 + (i * 107));
			deck->setYPos(700);

			deck->drawCard(player->getCard(i));
		}
	}

	if (playerSplit)
	{
		if (playerSplit->getCardCount() > 0)
		{
			for (int i = 0; i < playerSplit->getCardCount(); ++i)
			{
				deck->setXPos(1074 + (i * 107));
				deck->setYPos(700);

				deck->drawCard(playerSplit->getCard(i));
			}
		}
	}

	if (dealer->getCardCount() > 0)
	{
		for (int i = 0; i < dealer->getCardCount(); ++i)
		{
			deck->setXPos(150 + (i * 252));
			deck->setYPos(200);

			deck->drawCard(dealer->getCard(i));
		}
	}

	if (win)
	{
		panel->draw();
		victory->draw();
		quit->draw();
	}

	if (lose)
	{
		panel->draw();
		defeat->draw();
		quit->draw();
	}

	SDL_RenderPresent(renderer);
}

void shutdown()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();

	delete bg;
	delete table;
	delete deck;
	delete play;
	delete quit;
	delete victory;
	delete defeat;
	delete panel;
}