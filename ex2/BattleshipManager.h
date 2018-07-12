#pragma once
#include "BattleShipPlayer.h"
#include "Types.h"
#include "BattleshipAnimationUtils.h"
using namespace std;

// define function of the type we expect
typedef IBattleshipGameAlgoCommon * (*GetAlgorithmType)();
extern GetAlgorithmType getAlgoFunc;

class BattleshipManager
{
	string path_;
	BattleshipAnimationUtils * utils_;
	vector<BattleShipPlayer *> players;
	BattleShipNavy * navy;
	int turn;
	const int rows_, cols_;

	struct SetupManager
	{
		string board_="", a_dll="", b_dll="";
		bool error() const { return board_.empty() || a_dll.empty() || b_dll.empty(); }
		void parseDirLine(const string& line);
		bool hasEnding(string const &fullString, string const &ending);
	} setup_manager;

	void parseBoardLine(char** board, int row, const string& line);
	void printInitialBoard(char** board);
	bool createArray(ifstream& board_file);
	void buildNavy(const char** board);
	bool isGameOver(vector<bool> empties);
	void showResults(int winner) const;

public:
	BattleshipManager(const string& path, int rows = 10, int cols = 10);
	~BattleshipManager();

	BattleshipAnimationUtils* utils() const	{ return utils_; }

	bool setup();
	bool loadDLLs(const string& a, const string& b);
	void play(char** board);
};