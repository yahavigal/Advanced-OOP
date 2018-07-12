#pragma once
#include "IBattleshipGameAlgo.h"
#include "Types.h"

class IBattleshipGameAlgoCommon : public IBattleshipGameAlgo
{
protected:
	char** matrix;
	int rows, cols;
	int Player;

public:
	virtual ~IBattleshipGameAlgoCommon();
	virtual void setBoard(int player, const char** board, int numRows, int numCols) override;
	virtual bool init(const std::string& path) = 0;
	virtual std::pair<int, int> attack() = 0;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) = 0;
};