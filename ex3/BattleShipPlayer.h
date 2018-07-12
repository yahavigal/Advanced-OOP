#ifndef BATTLESHIP_PLAYER_H
#define BATTLESHIP_PLAYER_H

#pragma once

#include "IBattleshipGameAlgo.h"
#include "BattleShipLayouts.h"

using namespace std;

class BattleShipPlayer
{
	shared_ptr<IBattleshipGameAlgo> algo_;
	int score_;

public:
	BattleShipPlayer(int player) :score_(0), player_(player) {}
	virtual ~BattleShipPlayer() {}
	void award(int points) { score_ += points; }
	int player_;
	int getScore() const { return score_; }
	shared_ptr<IBattleshipGameAlgo> getAlgo() { return algo_; }
	void setAlgo(shared_ptr<IBattleshipGameAlgo> algo) { algo_ = algo; }
};

#endif