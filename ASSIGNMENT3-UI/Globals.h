#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cmath>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "Sprite.h"
#include "Deck.h"
#include "Player.h"
#include "Button.h"

using namespace std;

#define SCREEN_WIDTH 2048
#define SCREEN_HEIGHT 1536

const float FPS = 60.0f;
const float TICKSPERFRAME = 1000.0f / FPS;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

Sprite* bg = nullptr;
Sprite* table = nullptr;
Sprite* victory = nullptr;
Sprite* defeat = nullptr;
Sprite* panel = nullptr;

Deck* deck = nullptr;

Button* play = nullptr;
Button* quit = nullptr;
Button* hit = nullptr;
Button* stand = nullptr;
Button* pass = nullptr;
Button* doubleDown = nullptr;
Button* split = nullptr;


Player* player;
Player* dealer;
Player* playerSplit = nullptr;

bool done = false;
bool win = false;
bool lose = false;