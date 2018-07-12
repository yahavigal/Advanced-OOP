#include "BattleshipAnimationUtils.h"

using namespace std;

BattleshipAnimationUtils::~BattleshipAnimationUtils()
{
	/* Restore Window's defaults */
	setCursorVisibility(true);
	revertColor();
}

void BattleshipAnimationUtils::setCursorVisibility(bool visible)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = visible;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void BattleshipAnimationUtils::gotoxy(int y, int x)
{
	COORD coord; // coordinates
	coord.X = x; coord.Y = y; // X and Y coordinates
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); // moves to the coordinates
}

void BattleshipAnimationUtils::setColor(int player)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	while (hStdout == INVALID_HANDLE_VALUE)
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	/* Select the relevant color */
	if (player == 0)
		SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
	else if (player == 1)
		SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN);
	else
		SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE);
}

void BattleshipAnimationUtils::revertColor()
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	while (hStdout == INVALID_HANDLE_VALUE)
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int BattleshipAnimationUtils::wherex() const
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&csbi))
		return -1;
	return csbi.dwCursorPosition.X;
}

int BattleshipAnimationUtils::wherey() const
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&csbi))
		return -1;
	return csbi.dwCursorPosition.Y;
}

void BattleshipAnimationUtils::setXY(int x, int y)
{
	cellX = x;
	cellY = y;
}

void BattleshipAnimationUtils::printBoard(AttackResult res, Point point) {
	Point xy = { cellX + point.first-1, cellY + point.second-1 };
	gotoxy(xy.first, xy.second);
	Sleep(delay);
	setColor(2);
	cout << "@" << flush;
	Sleep(delay / 2);
	gotoxy(xy.first, xy.second);
	if (res == AttackResult::Miss)
	{
		cout << " " << flush;
		return;
	}
	cout << "*" << flush;
}