#include "BattleShipLayouts.h"

using namespace std;

// *********************************** //
//   BattleShipPiece::BattleShipRect   //
// *********************************** //

BattleShipPiece::BattleShipRect::BattleShipRect(const Coordinate & p, BattleShipModel m, ShipOrientation o) :area_(p, p), ori_(o), hit_(m.length, false)
{
	if (ori_ == Horizontal) area_.second = Coordinate(p.row, p.col + m.length - 1,p.depth);
	else if(ori_ == Vertical) area_.second = Coordinate(p.row + m.length - 1, p.col,p.depth);
	else area_.second = Coordinate(p.row, p.col, p.depth + m.length - 1);
}

tuple<Rect, Rect, Rect> BattleShipPiece::BattleShipRect::safeZone() const
{
	Rect r1(Coordinate(0,0,0), Coordinate(0, 0, 0)), r2(Coordinate(0, 0, 0), Coordinate(0, 0, 0)), r3(Coordinate(0, 0, 0), Coordinate(0, 0, 0));
	Coordinate top_left1(area_.first.row, area_.first.col - 1, area_.first.depth);
	Coordinate bottom_right1(area_.second.row, area_.second.col + 1, area_.second.depth);
	r1 = Rect(top_left1, bottom_right1);
	Coordinate top_left2(area_.first.row - 1, area_.first.col, area_.first.depth);
	Coordinate bottom_right2(area_.second.row + 1, area_.second.col, area_.second.depth);
	r2 = Rect(top_left2, bottom_right2);
	Coordinate top_left3(area_.first.row, area_.first.col, area_.first.depth - 1);
	Coordinate bottom_right3(area_.second.row, area_.second.col, area_.second.depth + 1);
	r3 = Rect(top_left3, bottom_right3);
	return make_tuple(r1, r2, r3);
}

bool BattleShipPiece::BattleShipRect::contains(const Coordinate& p)
{
	return area_.first.row <= p.row
		&& area_.second.row >= p.row
		&& area_.first.col <= p.col
		&& area_.second.col >= p.col
		&& area_.first.depth <= p.depth
		&& area_.second.depth >= p.depth;
}

bool BattleShipPiece::BattleShipRect::overlaps(const BattleShipRect& r)
{
	tuple<Rect, Rect, Rect> safe_zone = r.safeZone();
	return (area_.first.col <= get<0>(safe_zone).second.col
		&& area_.second.col >= get<0>(safe_zone).first.col
		&& area_.first.row <= get<0>(safe_zone).second.row
		&& area_.second.row >= get<0>(safe_zone).first.row
		&& area_.first.depth <= get<0>(safe_zone).second.depth
		&& area_.second.depth >= get<0>(safe_zone).first.depth)
		|| (area_.first.col <= get<1>(safe_zone).second.col
			&& area_.second.col >= get<1>(safe_zone).first.col
			&& area_.first.row <= get<1>(safe_zone).second.row
			&& area_.second.row >= get<1>(safe_zone).first.row)
		&& area_.first.depth <= get<1>(safe_zone).second.depth
		&& area_.second.depth >= get<1>(safe_zone).first.depth
		|| (area_.first.col <= get<2>(safe_zone).second.col
			&& area_.second.col >= get<2>(safe_zone).first.col
			&& area_.first.row <= get<2>(safe_zone).second.row
			&& area_.second.row >= get<2>(safe_zone).first.row
			&& area_.first.depth <= get<2>(safe_zone).second.depth
			&& area_.second.depth >= get<2>(safe_zone).first.depth);
}

int BattleShipPiece::BattleShipRect::coorToIndex(const Coordinate & p)
{
	if (ori_ == Horizontal) return p.col - area_.first.col;
	else if (ori_ == Vertical) return p.row - area_.first.row;
	return p.depth- area_.first.depth;
}


// ******************* //
//   BattleShipPiece   //
// ******************* //

bool BattleShipPiece::isSunk() const
{
	for (size_t i = 0; i < rect_.hit_.size(); i++)
		if (!rect_.hit_[i]) return false;
	return true;
}

AttackResult BattleShipPiece::attack(const Coordinate & p)
{
	if (!rect_.contains(p)) return AttackResult::Miss;
	rect_.hit_[rect_.coorToIndex(p)] = true;
	if (isSunk()) return AttackResult::Sink;
	return AttackResult::Hit;
}

bool BattleShipPiece::overlaps(const shared_ptr<BattleShipPiece> & ship)
{
	return rect_.overlaps(ship->rect_);
}


// ****************** //
//   BattleShipNavy   //
// ****************** //

BattleShipNavy::~BattleShipNavy()
{
	ships_.clear();
}

void BattleShipNavy::addBattleShip(const shared_ptr<BattleShipPiece> & ship)
{
	ships_.push_back(ship);
	sunkships.push_back(false);
	if (ship->player() == 0) aShips++;
	else bShips++;
}

AttackResult BattleShipNavy::attack(const Coordinate & p)
{

	AttackResult res;
	Coordinate p_ = { p.row - 1, p.col - 1,p.depth-1 };
	for (size_t i = 0; i < ships_.size(); i++)
	{
		if (sunkships[i]) continue;
		res = ships_[i]->attack(p_);
		if (res == AttackResult::Hit) return res;
		if (res == AttackResult::Sink)
		{
			sunkships[i] = true;
			if (ships_[i]->player() == 0)
				aShips--;
			else
				bShips--;
			return AttackResult::Sink;
		}
	}

	return AttackResult::Miss;
}

BattleShipModel BattleShipNavy::modelAt(const Coordinate& p)
{
	for (size_t t = 0; t < ships_.size(); t++){
		if (ships_[t]->contains(Coordinate{ p.row - 1, p.col - 1, p.depth - 1 }))
			return ships_[t]->model();
	}

	return{ -1, -1 };
}

int BattleShipNavy::defeated() const
{
	if (aShips == 0) return 1;
	if (bShips == 0) return 0;
	return -1;
}

bool BattleShipNavy::isValid() const
{
	bool error = false;
	// Check navy size (should be exactly 5)

	// Check for no overlaps
	for (size_t i = 0; i < ships_.size(); i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			if (ships_[i]->overlaps(ships_[j]))
			{
				cout << "Adjacent Ships on Board" << endl;
				return false;
			}
		}
	}
	cout << aShips << " " << bShips << endl;

	if (!error)	return true;
	return false;
}

bool BattleShipNavy::isContinuousShip(char*** board, const Coordinate& p, char m, int model_size, BattleShipPiece::ShipOrientation orientation)
{
	size_t x, y;

	if (orientation == BattleShipPiece::ShipOrientation::Horizontal) {
		for (x = p.col + 1; x < ((size_t)p.col + (size_t)model_size); x++) {
			if (board[p.row][x][p.depth] != m)
				return false;
		}
	}

	else if (orientation == BattleShipPiece::ShipOrientation::Vertical) {
		for (y = p.row + 1; y < ((size_t)p.row + (size_t)model_size); y++) {
			if (board[y][p.col][p.depth] != m)
				return false;
		}
	}
	else {
		for (y = p.depth + 1; y < ((size_t)p.depth + (size_t)model_size); y++) {
			if (board[p.row][p.col][y] != m)
				return false;
		}
	}
	return true;
}

bool BattleShipNavy::isOverlappingItself(char*** board, const Coordinate& p, char m, int model_size, BattleShipPiece::ShipOrientation orientation)
{
	size_t x, y;

	/* Check if ship is too big */
	if (orientation == BattleShipPiece::ShipOrientation::Horizontal) {
		if ((p.col > 0 && board[p.row][p.col - 1][p.depth] == m)
			|| (p.col + model_size < cols_ - 1 && board[p.row][p.col + model_size][p.depth] == m))
			return true;

	}
	if (orientation == BattleShipPiece::ShipOrientation::Vertical) {
		if ((p.row > 0 && board[p.row - 1][p.col][p.depth] == m)
			|| (p.row + model_size < rows_ - 1 && board[p.row + model_size][p.col][p.depth] == m))
			return true;
	}
	if (orientation == BattleShipPiece::ShipOrientation::Depthical) {
		if ((p.depth > 0 && board[p.row][p.col][p.depth - 1] == m)
			|| (p.depth + model_size < depth_ - 1 && board[p.row][p.col][p.depth + model_size] == m))
			return true;
	}

	/* Check for identical adjacent ship */
	if (orientation == BattleShipPiece::ShipOrientation::Horizontal) {
		for (x = p.col; x < (size_t)p.col + model_size; x++)
		{
			if ((p.row > 0 && board[p.row - 1][x][p.depth] == m)
				|| (p.row < rows_ - 1 && board[p.row + 1][x][p.depth] == m))
				return true;
			if ((p.depth > 0 && board[p.row][x][p.depth - 1] == m)
				|| (p.depth < depth_ - 1 && board[p.col][x][p.depth + 1] == m))
				return true;
		}
	}

	if (orientation == BattleShipPiece::ShipOrientation::Vertical) {
		for (y = p.row; y < (size_t)p.row + model_size; y++)
		{
			if ((p.col > 0 && board[y][p.col - 1][p.depth] == m)
				|| (p.col < cols_ - 1 && board[y][p.col + 1][p.depth] == m))
				return true;
			if ((p.depth > 0 && board[y][p.col][p.depth - 1] == m)
				|| (p.depth < depth_ - 1 && board[y][p.col][p.depth + 1] == m))
				return true;
		}
	}
	if (orientation == BattleShipPiece::ShipOrientation::Depthical) {
		for (y = p.depth; y < (size_t)p.depth + model_size; y++)
		{
			if ((p.row > 0 && board[p.row - 1][p.col][y] == m)
				|| (p.row < rows_ - 1 && board[p.row + 1][p.col][y] == m))
				return true;
			if ((p.col > 0 && board[p.row][p.col - 1][y] == m)
				|| (p.col < cols_ - 1 && board[p.row][p.col + 1][y] == m))
				return true;
		}
	}

	return false;
}

int BattleShipNavy::isValidShipAt(char*** board, const Coordinate& p, char m)
{
	int player = 1 - !islower(m);
	int model_size = BattleShipModel::fromChar(m).length;
	BattleShipPiece::ShipOrientation orientation;

	// Check for horizontal ship
	if (p.col <= cols_ - model_size && board[p.row][p.col + model_size - 1][p.depth] == m)
		orientation = BattleShipPiece::ShipOrientation::Horizontal;

	// Check for vertical ship
	else if (p.row <= rows_ - model_size && board[p.row + model_size - 1][p.col][p.depth] == m)
		orientation = BattleShipPiece::ShipOrientation::Vertical;
	else if (p.depth <= depth_ - model_size && board[p.row][p.col][p.depth + model_size - 1] == m)
		orientation = BattleShipPiece::ShipOrientation::Depthical;
	else {
		if (!error_shown[4 * player + model_size - 1]) {
			cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
			error_shown[4 * player + model_size - 1] = true;
		}
		return -1;
	}

	bool valid = isContinuousShip(board, p, m, model_size, orientation) &&
		!isOverlappingItself(board, p, m, model_size, orientation);

	if (!valid) {
		if (!error_shown[4 * player + model_size - 1]) {
			cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
			error_shown[4 * player + model_size - 1] = true;
		}
		return -1;
	}

	return orientation;
}

char BattleshipBoard::charAt(Coordinate c) const
{
	return mBoard[c.row - 1][c.col - 1][c.depth - 1];
}

void BattleshipBoard::put(const Coordinate & c, char ch)
{
	mBoard[c.row - 1][c.col - 1][c.depth - 1] = ch;
}

vector<shared_ptr<BattleShipPiece>> BattleShipNavy::getShipsVec() const
{
	return ships_;
}

void BattleshipBoard::parseBoardLine(int depth, int row, const string& line)
{
	int bound = min((int)line.length(), cols());

	for (int j = 0; j < bound; j++)
		put({ j + 1, row + 1, depth + 1 }, line[j]);

	for (int j = bound; j < cols(); j++)
		put({ j + 1, row + 1, depth + 1 }, ' ');
}

tuple<string, string> BattleshipBoard::buildNavy()
{
	string msg1 = "", msg2 = "";
	int a[4] = { 0, 0, 0, 0 };
	int b[4] = { 0, 0, 0, 0 };
	for (BattleShipModel m : BattleShipModel::modelsToArray())
	{
		for (int x = 0; x < _rows; x++){
			for (int y = 0; y < _cols; y++) {
				for (int z = 0; z < _depth; z++) {
					if (m == BattleShipModel::fromChar(mBoard[x][y][z]))
					{
						if (x > 0 && BattleShipModel::fromChar(mBoard[x - 1][y][z]) == m) //this ship has been checked
							continue;
						if (y > 0 && BattleShipModel::fromChar(mBoard[x][y - 1][z]) == m)
							continue;
						if (z > 0 && BattleShipModel::fromChar(mBoard[x][y][z - 1]) == m)
							continue;
						int valid = mNavy.isValidShipAt(mBoard, { x, y, z }, mBoard[x][y][z]);
						if (valid != -1)
						{
							if (mBoard[x][y][z] == tolower(mBoard[x][y][z]))
								mNavy.addBattleShip(make_shared<BattleShipPiece>(Coordinate(x, y, z), m, (BattleShipPiece::ShipOrientation)valid, 1));
							else mNavy.addBattleShip(make_shared<BattleShipPiece>(Coordinate(x, y, z), m, (BattleShipPiece::ShipOrientation)valid, 0));
						}
					}
				}
			}
		}
	}
	for (shared_ptr<BattleShipPiece> bsp : mNavy.getShipsVec()){
		if (bsp->model() == BattleShipModel::B()){
			if (bsp->player() == 0) a[0]++;
			else b[0]++;
		}
		else if (bsp->model() == BattleShipModel::P()){
			if (bsp->player() == 0) a[1]++;
			else b[1]++;
		}
		else if (bsp->model() == BattleShipModel::M()){
			if (bsp->player() == 0) a[2]++;
			else b[2]++;
		}
		else{
			if (bsp->player() == 0) a[3]++;
			else b[3]++;
		}
	}
	for (int i = 0; i < 4; i++){
		if ((a[i] == 0 && b[i] != 0) || (a[i] != 0 && b[i] == 0))
			msg1 = "The board is imbalanced - different types of ships!";
		if (a[i] != 0 && b[i] != 0 && a[i] != b[i])
			msg2 = "The board is imbalanced - different amount of ships!";
	}
	return make_tuple(msg1, msg2);
}