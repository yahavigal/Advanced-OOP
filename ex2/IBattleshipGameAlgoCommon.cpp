#include "IBattleshipGameAlgoCommon.h"
using namespace std;

IBattleshipGameAlgoCommon::~IBattleshipGameAlgoCommon()
{
	for (int x = 0; x < rows; x++)
		free(matrix[x]);
	free(matrix);
}

void IBattleshipGameAlgoCommon::setBoard(int player, const char** board, int numRows, int numCols)
{
	Player = player;
	rows = numRows;
	cols = numCols;
	int offset = 32 * player;
	matrix = (char**)calloc(sizeof(char*), rows);
	for (size_t i = 0; i < rows; i++) {
		matrix[i] = (char*)calloc(sizeof(char), cols);
	}
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			char ch = board[i][j];
			matrix[i][j] =ch;
		}
	}
	/*for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			cout<<"1"<<(matrix[i][j]);
		}
		cout << endl;
	}*/
}