#include "Player.h"

Player::Player(std::shared_ptr<Game> game, Color color) :activeGame(game), playerColor(color) {}