#pragma once
#include "Types.h"
#include "IBattleshipGameAlgo.h"
#include <tchar.h>
#include <sdkddkver.h>
#include <windows.h>

class BattleshipAnimationUtils
{
	bool quiet;
	int delay;
	int cellX, cellY;

public:
	/* Set default parameters in c'tor */
	BattleshipAnimationUtils() :quiet(false), delay(500) {}
	~BattleshipAnimationUtils();

	BattleshipAnimationUtils(const BattleshipAnimationUtils& bau) = delete;
	BattleshipAnimationUtils& operator=(const BattleshipAnimationUtils& bau) = delete;

	bool isQuiet() const { return quiet; }
	int getDelay() const { return delay; }
	void setDelay(int ms) { delay = ms;	}
	void setQuiet()	{ quiet = true; }
	void setCursorVisibility(bool visible);
	void gotoxy(int y, int x);
	void setColor(int player);
	void revertColor();
	int wherex() const;
	int wherey() const;
	void setXY(int x, int y);
	void printBoard(AttackResult res, Point point);

	inline void skip(int x, int y)
	{
		gotoxy(cellX + x-1, y-1);
	}

};