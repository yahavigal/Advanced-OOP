#pragma once
#include "IBattleshipGameAlgoCommon.h"
using namespace std;

class NaiveAlgo : public IBattleshipGameAlgoCommon
{
	queue<Point> moves;

public:
	NaiveAlgo() {}
	virtual ~NaiveAlgo() {}
	virtual bool init(const std::string& path) override;
	virtual std::pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override {}

	friend IBattleshipGameAlgo* GetAlgorithm();
};

ALGO_API IBattleshipGameAlgo* GetAlgorithm();