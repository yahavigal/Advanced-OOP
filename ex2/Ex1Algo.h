#pragma once
#include "IBattleshipGameAlgoCommon.h"

using namespace std;

class Ex1Algo : public IBattleshipGameAlgoCommon
{
	char** matrix;
	int rows, cols;
	queue<Point> moves;

	void setMoves(ifstream& file);
	void parseLine(const string& line);

public:
	Ex1Algo() {}
	virtual ~Ex1Algo() {}

	virtual bool init(const std::string& path) override;
	virtual std::pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override {}
	virtual bool hasEnding(string const &fullString, string const &ending);
	friend IBattleshipGameAlgo* GetAlgorithm();
};

ALGO_API IBattleshipGameAlgo* GetAlgorithm();