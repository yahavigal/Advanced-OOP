#pragma once
#include "BattleShipPlayer.h"
#include "Types.h"

using namespace std;

class BattleshipManager
{
	BattleshipBoard Board;
	vector<shared_ptr<BattleShipPlayer>> players;
	const int rows_, cols_, depth_;
	int turn;

	bool isGameOver(vector<bool> empties);
	int BattleshipManager::createOwnArray();

	void initPlayers(shared_ptr<IBattleshipGameAlgo> algo1, shared_ptr<IBattleshipGameAlgo> algo2)
	{
		players[0]->setAlgo(algo1);
		players[1]->setAlgo(algo2);
	}

public:
	BattleshipManager(BattleshipBoard & pBoard, shared_ptr<IBattleshipGameAlgo> algo1, shared_ptr<IBattleshipGameAlgo> algo2)
		:Board(0, 0, 0), rows_(pBoard.rows()), cols_(pBoard.cols()), depth_(pBoard.depth()), turn(0)
	{
		players.push_back(make_shared<BattleShipPlayer>(0));
		players.push_back(make_shared<BattleShipPlayer>(1));
		Board = pBoard;
		initPlayers(algo1, algo2);
	}

	~BattleshipManager()
	{
		cout << "DELETE" << endl;
		players.clear();
	}

	tuple<int, int, int> BattleshipManager::Manage();
	int BattleshipManager::play();
};