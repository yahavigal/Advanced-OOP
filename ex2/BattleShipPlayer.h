#pragma once

#include "IBattleshipGameAlgoCommon.h"
#include "BattleShipLayouts.h"

using namespace std;

class BattleShipPlayer
{
	IBattleshipGameAlgoCommon * algo_;
	int score_;

public:
	BattleShipPlayer(int player) :score_(0), player_(player) {}
	virtual ~BattleShipPlayer() {}
	void award(int points) { score_ += points; }
	int player_;
	int getScore() const { return score_; }
	IBattleshipGameAlgoCommon* getAlgo() const { return algo_; }

	void setAlgo(IBattleshipGameAlgoCommon* algo) {	algo_ = algo; }
};



