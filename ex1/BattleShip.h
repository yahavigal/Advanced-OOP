#pragma once

#include "IBattleshipGameAlgo.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <queue>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <set>
#include <stdlib.h>
#include "BattleShipLayouts.h"

using namespace std;


class BattleShipPlayer : public IBattleshipGameAlgo
{
#define BOARD_SIZE 10

	char** board;
	int score_;
	queue<Point> moves_;
	int player_;

public:
	BattleShipPlayer(int player) :score_(0), player_(player) {}
	virtual ~BattleShipPlayer();
	bool setMoves(ifstream& file);
	virtual void setBoard(const char** matrix, int numRows, int numCols);
	void printBoard() { 
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				cout << board[i][j];
			}
			cout << endl;
		}
	}
	/*
	* Returns player's next attack move.
	*/
	virtual pair<int, int> attack();
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;
	bool getCell(const Point& p) {
		return board[p.first][p.second] != ' ';
	}
	void award(int points) {
		score_ += points;
	}
	int getScore() const { return score_; }
};




bool createArray(ifstream& file, BattleShipPlayer* player0, BattleShipPlayer* player1, BattleShipNavy* navy);
void dostuffwithfile(const string& path, BattleShipPlayer* player0, BattleShipPlayer* player1, BattleShipNavy* navy);
bool buildNavy(BattleShipNavy* navy, const Board& board);
