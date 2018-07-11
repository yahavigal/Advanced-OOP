
#include "BattleshipManager.h"
using namespace std;


tuple<int, int, int> BattleshipManager::Manage()
{
	int Winner = createOwnArray();
	return make_tuple(Winner, players[0]->getScore(), players[1]->getScore());
}

bool BattleshipManager::isGameOver(vector<bool> empties)
{
	return Board.getNavy().defeated() != -1 || (empties[0] && empties[1]);
}

int BattleshipManager::createOwnArray()
{
	string msg;
	for (int i = 0; i < 2; i++)
	{
		shared_ptr<BattleshipBoard> shipBoard(new BattleshipBoard(rows_, cols_, depth_));
		int offset = 32 * i;
		for (int m = 0; m < rows_; m++)
		{
			for (int j = 0; j < cols_; j++)
			{
				for (int g = 0; g < depth_; g++)
				{
					Coordinate c = Coordinate(m + 1, j + 1, g + 1);
					char ch = Board.charAt(c);
					if (ch == 'B' + offset)
					{
						shipBoard->put(c, ch);
						continue;
					}

					if (ch == 'P' + offset)
					{
						shipBoard->put(c, ch);
						continue;
					}

					if (ch == 'M' + offset)
					{
						shipBoard->put(c, ch);
						continue;
					}

					if (ch == 'D' + offset)
					{
						shipBoard->put(c, ch);
						continue;
					}

					// No ship in cell (i,j)
					shipBoard->put(c, ' ');
				}
			}
		}
		players[i]->getAlgo()->setPlayer(i);
		players[i]->getAlgo()->setBoard(*shipBoard);
	}
	
	return play();
}


int BattleshipManager::play()
{
	vector<bool> Empty = { false, false };
	vector<vector<vector<bool>>> moves(Board.rows(), vector<vector<bool>>(Board.cols(), vector<bool>(Board.depth(), false)));
	int Winner;
	Coordinate p(-1, -1, -1);
	while (true) {
		if (isGameOver(Empty)) {
			Winner = Board.getNavy().defeated();
			break;
		}
		p = players[turn]->getAlgo()->attack();
		if (p.row == -1 && p.depth == -1 && p.col == -1) {
			Empty[turn] = true;
			turn = 1 - turn;
		}
		else{
			moves[p.row - 1][p.col - 1][p.depth - 1] = true;
			AttackResult attack_res = Board.getNavy().attack(p);
			players[0]->getAlgo()->notifyOnAttackResult(turn, p, attack_res);
			players[1]->getAlgo()->notifyOnAttackResult(turn, p, attack_res);
			if (attack_res == AttackResult::Miss) {
				turn = 1 - turn;
				continue;
			}
			if (attack_res == AttackResult::Hit) {
				bool isOccuplied = ((!islower(Board.charAt({ p.row, p.col, p.depth }) && turn == 0) || ((islower(Board.charAt({ p.row, p.col, p.depth })) && turn == 1))));
				if (isOccuplied || !moves[p.row - 1][p.col - 1][p.depth - 1]) {
					turn = 1 - turn; //this is a own goal
					continue;
				}
				continue;//else: you have a second turn
			}
			bool isOccuplied = ((!Board.charAt({ p.row, p.col, p.depth }) && turn == 0) || (islower(Board.charAt({ p.row, p.col, p.depth }) && turn == 1)));
			int value = Board.getNavy().modelAt(p).value;
			if (attack_res == AttackResult::Sink && isOccuplied) {
				players[1 - turn]->award(value);
				turn = 1 - turn;
			}
			else
			{
				players[turn]->award(value);
			}
		}
	}
	return Winner;
}