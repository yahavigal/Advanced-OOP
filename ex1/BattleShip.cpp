#include "BattleShip.h"


bool createArray(ifstream& file, BattleShipPlayer* player0, BattleShipPlayer* player1, BattleShipNavy* navy) {
	char** board = (char **)calloc(sizeof(char*), 10);
	if (!board) {
		cout << "Error: Allocation error" << endl;
		return (bool)-1;
	}
	
	Board shared_board(10, vector<char>(10));
	string line;
	for (int i = 0; i < 10; i++) {
		board[i] = (char *)calloc(sizeof(char), 10);
		if (!board[i]) {
			cout << "Error: Allocation error" << endl;
			return (bool)-1;
		}

		getline(file, line);
		int bound = min((int)line.length(), 10);
		for (int j = 0; j < bound; j++)
		{
			board[i][j] = line[j];
			shared_board[i][j] = line[j];
		}
		for (int j = line.length(); j < 10; j++)
		{
			board[i][j] = ' ';
			shared_board[i][j] = ' ';
		}
	}

	buildNavy(navy, shared_board);
	player0->setBoard((const char**)board,10,10);
	player1->setBoard((const char**)board,10,10);

	for (int i = 0; i < 10; i++)
		free(board[i]);
	free(board);

	return navy->isValid();

}
bool BattleShipPlayer::setMoves(ifstream& file) {
	string line;
	int i = 0;
	while (getline(file,line))
	{
		bool line_dismissed = false;
			pair<int, int> p;
			bool commaFound = false;
			bool numberFound = false;
			string n1(""), n2("");
			for (size_t t = 0; t < line.size(); t++)
			{
				if (isspace(line[t])) continue;
				if (line[t] == ',') {
					commaFound = true;
					numberFound = false;
					p.first = atoi(n1.c_str())-1;
					continue;
				}
				if (!commaFound) {
					if (!numberFound) {
						if ((line[t] < '0') || (line[t] > '9')) {
							line_dismissed = true;
							break;
						}
						n1 += line[t];
						numberFound = true;
					}
					else {
						if (line[t] != '0') {
							line_dismissed = true;
							break;
						}
						n1 += line[t];
					}
				}
				else {
					if (!numberFound) {
						if ((line[t] < '0') || (line[t] > '9')) {
							line_dismissed = true;
							break;
						}
						n2 += line[t];
						numberFound = true;
					}
					else {
						if (line[t] != '0') {
							line_dismissed = true;
							break;
						}
						n2 += line[t];
					}
				}
			}
			if (line_dismissed || (!commaFound && !numberFound)) continue; // to ignore empty lines
			p.second = atoi(n2.c_str())-1;

			moves_.push(p);
		}		
		

	return true;
}


int LetsPlay(vector<BattleShipPlayer*> players, BattleShipNavy* navy) {
	int turn = 0;
	vector<bool> Empty = { false,false };
	int i = -1;
	set<Point> moves;
	while (true) {
		if (navy->defeated() != -1) return navy->defeated();
		i++;
		if (Empty[0] && Empty[1]) break;
		Point p = players[turn]->attack();
		if (p == Point(-1, -1)) {
			turn = 1 - turn;
			Empty[turn] = true;
		}
		else {
			pair<set<Point>::iterator, bool> ins = moves.insert(p);
			AttackResult attack_res = navy->attack(p);
			if (attack_res == AttackResult::Miss){
				turn = 1 - turn;
				continue;
			}
			if (attack_res == AttackResult::Hit) {
				if (players[turn]->getCell(p) || !ins.second) {
					turn = 1 - turn; //this is a own goal
					continue;
				}
				continue;

				//else: you have a second turn
			}
			int value = navy->modelAt(p)->value();
			if (attack_res == AttackResult::Sink && players[turn]->getCell(p)) {
				players[1 - turn]->award(value);
				turn = 1 - turn;
			}
			else
			{
				players[turn]->award(value);
			}

		}
		
	}		

	return -1;
}

void BattleShipPlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	cout << "Player " << (player ? "B" : "A") << "'s move: (" << row << "," << col << ")\tResult: ";
	switch (result)
	{
	case(AttackResult::Hit):
		cout << "Hit" << endl;
		break;
	case(AttackResult::Miss):
		cout << "Miss" << endl;
		break;
	case(AttackResult::Sink):
		cout << "Sink" << endl;
		break;
	}


}

BattleShipPlayer::~BattleShipPlayer()
{
	if (board != nullptr) {
		for (size_t t = 0; t < BOARD_SIZE; t++)
			free(board[t]);
		free(board);
	}
}
bool buildNavy(BattleShipNavy* navy, const Board& board)
{
	for (BattleShipModel * m : modelsToArray())
	{
		for (size_t x = 0; x < board.size(); x++)
			for (size_t y = 0; y < board.size(); y++)
				if (m == fromChar(board[x][y]))
				{
					int valid = navy->isValidShipAt(board, Point(x, y), board[x][y]);
					if (valid != -1)
					{
						if (board[x][y] == tolower(board[x][y]))
							navy->addBattleShip(new BattleShipPiece(Point(x, y), m, (BattleShipPiece::ShipOrientation)valid, 1));
						else navy->addBattleShip(new BattleShipPiece(Point(x, y), m, (BattleShipPiece::ShipOrientation)valid, 0));
					}
				}

		
	}
	return true;	// placeholde
}

void BattleShipPlayer::setBoard(const char** matrix, int numRows, int numCols)
{
	int offset = 32 * player_;
	board = (char**)calloc(sizeof(char*), BOARD_SIZE);
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		board[i] = (char*)calloc(sizeof(char), BOARD_SIZE);
	}
	for (size_t i = 0; i < BOARD_SIZE; i++)
	{
		for (size_t j = 0; j < BOARD_SIZE; j++)
		{
			char ch = matrix[i][j];

			if (ch == 'B' + offset)
			{
				board[i][j] = 'B' + offset;
				continue;
			}

			if (ch == 'P' + offset)
			{
				board[i][j] = 'P' + offset;
				continue;
			}

			if (ch == 'M' + offset)
			{
				board[i][j] = 'M' + offset;
				continue;
			}

			if (ch == 'D' + offset)
			{
				board[i][j] = 'D' + offset;
				continue;
			}

			// No ship in cell (i,j)
			board[i][j] = ' ';
		}
	}
}

pair<int, int> BattleShipPlayer::attack()
{
	if (moves_.empty()) return Point(-1, -1);
	Point p = moves_.front();
	moves_.pop();

	return p;
}



void dostuffwithfile(const string& path, BattleShipPlayer* player0, BattleShipPlayer* player1, BattleShipNavy* navy)
{
	bool error = false;
	ifstream names("filenames.txt");
	string boardsuffix(".sboard"), asuffix(".attack-a"), bsuffix(".attack-b");
	bool board_found = false, a_found = false, b_found = false;
	string board, a, b;
	string curr_file;
	while (getline(names, curr_file) && (!board_found || !a_found || !b_found))
	{
		if (!board_found && curr_file.find(boardsuffix) != string::npos)
		{
			board_found = true;
			board = curr_file;
			continue;
		}

		if (!a_found && curr_file.find(asuffix) != string::npos)
		{
			a_found = true;
			a = curr_file;
			continue;
		}

		if (!b_found && curr_file.find(bsuffix) != string::npos)
		{
			b_found = true;
			b = curr_file;
			continue;
		}
	}

	if (!board_found)
	{
		error = true;
		cout << "Missing board file (*.sboard) looking in path: " << path << endl;
	}
	if (!a_found) {
		error = true;
		cout << "Missing attack file for player A (*.attack-a) looking in path: " << path << endl;
	}
	if (!b_found) {
		error = true;
		cout << "Missing attack file for player B (*.attack-b) looking in path: " << path << endl;
	}
	if (error) return;
	string prefixPath;
	if (path != " ")
		prefixPath = path + "\\";
	else
		prefixPath = "";
	if (!createArray(ifstream(prefixPath + board), player0, player1, navy)) { return; }
	player0->setMoves(ifstream(prefixPath + a));
	player1->setMoves(ifstream(prefixPath + b));
	//handle case a/b files not found
	int resGame = LetsPlay({ player0,player1 }, navy);
	if (resGame == 0) {
		cout << "Player A won" << endl;
	}
	if (resGame == 1) {
		cout << "Player B won" << endl;
	}
	cout << "Points:" << endl;
	cout << "Player A: "<<player0->getScore() << endl;
	cout << "Player B: "<< player1->getScore() << endl;
}