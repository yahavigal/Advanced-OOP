#include "BattleshipManager.h"
using namespace std;
GetAlgorithmType getAlgoFunc;

//from :http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c
bool BattleshipManager::SetupManager::hasEnding(string const &fullString, string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

void BattleshipManager::SetupManager::parseDirLine(const string& line)
{
	if (board_.empty() && hasEnding(line, ".sboard"))
		board_ = line;

	if (a_dll.empty() && hasEnding(line, ".dll")) {
		a_dll = line;
		return;
	}

	if (b_dll.empty() && hasEnding(line, ".dll"))
		b_dll = line;
}

void BattleshipManager::parseBoardLine(char** board, int row, const string& line)
{
	int bound = min((int)line.length(), cols_);
	for (int j = 0; j < bound; j++)
	{
		board[row][j] = line[j];
	}
	for (size_t j = line.length(); j < cols_; j++)
	{
		board[row][j] = ' ';
	}
}

void BattleshipManager::printInitialBoard(char** board)
{
	if (!utils_->isQuiet()) {
		utils_->setXY(utils_->wherey(), utils_->wherex());
		for (int i = 0; i < rows_; i++) {
			for (int j = 0; j < cols_; j++) {
				utils_->setColor(board[i][j] == tolower(board[i][j]));
				cout << board[i][j];

			}
			cout << endl;
		}
	}
}

bool BattleshipManager::createArray(ifstream& board_file)
{
	char** board;
	bool flag = false, Valid;
	if (setup_manager.board_.empty())
		flag = true;

	if (!flag) {
		board = (char**)calloc(rows_, sizeof(char*));
		if (!board)
		{
			cout << "Error: Allocation error." << endl;
			return false;
		}

		for (int t = 0; t < rows_; t++)
		{
			board[t] = (char *)calloc(cols_, sizeof(char));
			if (!board[t])
			{
				cout << "Error: Allocation error." << endl;
				return false;
			}
		}
		string line;
		for (int i = 0; i < rows_; i++) {
			getline(board_file, line);
			parseBoardLine(board, i, line);
		}
		buildNavy((const char**)board);
		Valid = navy->isValid();
	}
	if (setup_manager.b_dll.empty())
	{
		cout << "Missing an algorithm (dll) file looking in path: " << path_ << endl;
		return false;
	}
	if (flag) return false;

	bool dll_loaded = loadDLLs(setup_manager.a_dll, setup_manager.b_dll);
	if (!dll_loaded)
		return false;
	string msg;
	char **matrix;
	for (int i = 0; i < 2; i++) {
		int offset = 32 * i;
		matrix = (char**)calloc(sizeof(char*), rows_);
		for (size_t s = 0; s < rows_; s++) {
			matrix[s] = (char*)calloc(sizeof(char), cols_);
		}
		for (size_t m = 0; m < rows_; m++)
		{
			for (size_t j = 0; j < cols_; j++)
			{
				char ch = board[m][j];

				if (ch == 'B' + offset)
				{
					matrix[m][j] = 'B' + offset;
					continue;
				}

				if (ch == 'P' + offset)
				{
					matrix[m][j] = 'P' + offset;
					continue;
				}

				if (ch == 'M' + offset)
				{
					matrix[m][j] = 'M' + offset;
					continue;
				}

				if (ch == 'D' + offset)
				{
					matrix[m][j] = 'D' + offset;
					continue;
				}

				// No ship in cell (i,j)
				matrix[m][j] = ' ';
			}
		}

		players[i]->getAlgo()->setBoard(i, (const char**)matrix, rows_, cols_);
		for (int k = 0; k < rows_; k++) {
			free(matrix[k]);
		}
		free(matrix);
		const unsigned long maxDir = 260;
		char currentDir[maxDir];
		GetCurrentDirectoryA(maxDir, currentDir);
		if (path_ == "") {
			path_ = string(currentDir);
		}

		if (!players[i]->getAlgo()->init(path_)) {
			if (i == 0)
				msg = setup_manager.a_dll;
			else
				msg = setup_manager.b_dll;
			cout << "Algorithm initialization failed for dll: " << msg << endl;
			return false;
		}
	}
	if (Valid == true)
		printInitialBoard(board);
	play(board);
	for (int i = 0; i < rows_; i++)
		free(board[i]);
	free(board);
	return Valid;
}

void BattleshipManager::buildNavy(const char** board)
{
	for (BattleShipModel * m : BattleShipModel::modelsToArray())
	{
		for (int x = 0; x < rows_; x++)
			for (int y = 0; y < cols_; y++) {
				if (m == BattleShipModel::fromChar(board[x][y]))
				{
					if (x > 0 && BattleShipModel::fromChar(board[x - 1][y]) == m) //this ship has been checked
						continue;
					if (y > 0 && BattleShipModel::fromChar(board[x][y - 1]) == m)
						continue;
					int valid = navy->isValidShipAt(board, { x,y }, board[x][y]);
					if (valid != -1)
					{
						if (board[x][y] == tolower(board[x][y]))
							navy->addBattleShip(new BattleShipPiece(Point(x, y), m, (BattleShipPiece::ShipOrientation)valid, 1));
						else navy->addBattleShip(new BattleShipPiece(Point(x, y), m, (BattleShipPiece::ShipOrientation)valid, 0));
					}
				}
			}
	}
}

void BattleshipManager::showResults(int winner) const {
	if (!utils_->isQuiet()) {
		utils_->skip(rows_, cols_);
		cout << endl;
		utils_->revertColor();
	}

	if (winner == 0) {
		cout << "Player A won" << endl;
	}
	if (winner == 1) {
		cout << "Player B won" << endl;
	}
	cout << "Points:" << endl;
	cout << "Player A: " << players[0]->getScore() << endl;
	cout << "Player B: " << players[1]->getScore();
}

BattleshipManager::BattleshipManager(const string& path, int rows, int cols) :path_(path), utils_(new BattleshipAnimationUtils()), rows_(rows), cols_(cols), navy(new BattleShipNavy(rows, cols)), turn(0)
{
	players = { new BattleShipPlayer(0), new BattleShipPlayer(1) };
	utils_->setCursorVisibility(false);
}

BattleshipManager::~BattleshipManager()
{
	players.clear();
	delete utils_;
}

bool BattleshipManager::setup()
{
	struct stat dir;
	if (!path_.empty() && stat(path_.c_str(), &dir)) {
		cout << "Wrong path: " << path_ << endl;;
		return false;
	}
	char buffer[4096];
	std::string data_str;
	string arg = "2>NUL dir /b /a-d \"" + path_ + "\"";
	FILE* fp = _popen(arg.c_str(), "r");

	while (fgets(buffer, 4095, fp))
		data_str += std::string(buffer);
	std::string line;
	std::istringstream data_str2(data_str);
	while (std::getline(data_str2, line))
		setup_manager.parseDirLine(line);

	_pclose(fp);
	if (setup_manager.board_.empty())
		cout << "Missing board file (*.sboard) looking in path: " << path_ << endl;

	string prefixPath;
	if (path_ != "")
		prefixPath = path_ + "/";
	else
		prefixPath = "";

	setup_manager.a_dll = prefixPath + setup_manager.a_dll;
	if (!setup_manager.b_dll.empty())
		setup_manager.b_dll = prefixPath + setup_manager.b_dll;
	return createArray(ifstream(prefixPath + setup_manager.board_));
}

bool BattleshipManager::loadDLLs(const string& a, const string& b)
{
	// Load dynamic library

	HINSTANCE ADll = LoadLibraryA(a.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!ADll)
	{
		cout << "Cannot load dll : " << a << endl;
		return false;
	}
	// Get function pointer
	getAlgoFunc = (GetAlgorithmType)GetProcAddress(ADll, "GetAlgorithm");

	if (!getAlgoFunc)
	{
		cout << "Error: Cannot find GetAlgorithm function pointer from dll: " << path_ << "//" << a << endl;
		return false;
	}
	players[0]->setAlgo(getAlgoFunc());
	HINSTANCE BDll = LoadLibraryA(b.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!BDll)
	{
		cout << "Cannot load dll : " << path_ << "//" << b << endl;
		return false;
	}

	getAlgoFunc = (GetAlgorithmType)GetProcAddress(BDll, "GetAlgorithm");
	players[1]->setAlgo(getAlgoFunc());

	return true;
}

bool BattleshipManager::isGameOver(vector<bool> empties)
{
	return navy->defeated() != -1 || (empties[0] && empties[1]);
}

void BattleshipManager::play(char** board) {
	vector<bool> Empty = { false,false };
	set<Point> moves;
	while (true) {
		if (isGameOver(Empty)) {
			showResults(navy->defeated());
			break;
		}
		Point p = players[turn]->getAlgo()->attack();
		if (p == Point(-1, -1)) {
			Empty[turn] = true;
			turn = 1 - turn;
		}
		else {
			pair<set<Point>::iterator, bool> ins = moves.insert(p);
			AttackResult attack_res = navy->attack(p);

			if (ins.second && !utils_->isQuiet())
				utils_->printBoard(attack_res, p);
			for (auto Pl : players) {
				Pl->getAlgo()->notifyOnAttackResult(turn, p.first, p.second, attack_res);
			}
			if (attack_res == AttackResult::Miss) {
				turn = 1 - turn;
				continue;
			}
			if (attack_res == AttackResult::Hit) {
				bool isOccuplied = ((!islower(board[p.first - 1][p.second - 1]) && turn == 0) || (islower(board[p.first - 1][p.second - 1]) && turn == 1));
				//players[turn]->getAlgo()->isOccupied(p);
				if (isOccuplied || !ins.second) {
					turn = 1 - turn; //this is a own goal
					continue;
				}
				continue;

				//else: you have a second turn
			}
			bool isOccuplied = (!islower(board[p.first - 1][p.second - 1]) && turn == 0) || (islower(board[p.first - 1][p.second - 1]) && turn == 1);
			int value = navy->modelAt(p)->value();
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
}