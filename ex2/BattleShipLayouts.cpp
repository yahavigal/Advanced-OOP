#include "BattleShipLayouts.h"

using namespace std;

// *********************************** //
//   BattleShipPiece::BattleShipRect   //
// *********************************** //

BattleShipPiece::BattleShipRect::BattleShipRect(const Point & p, BattleShipModel * m, ShipOrientation o) :ori_(o), hit_(m->size(), false)
{
	area_.first = p;
	if (ori_ == Horizontal) area_.second = Point(p.first, p.second + m->size() - 1);
	else area_.second = Point(p.first + m->size() - 1, p.second);
}

pair<Rect, Rect> BattleShipPiece::BattleShipRect::safeZone() const
{
	Rect r1, r2;
	Point top_left1(area_.first.first, area_.first.second - 1);
	Point bottom_right1(area_.second.first, area_.second.second + 1);
	r1 = Rect(top_left1, bottom_right1);
	Point top_left2(area_.first.first - 1, area_.first.second);
	Point bottom_right2(area_.second.first + 1, area_.second.second);
	r2 = Rect(top_left2, bottom_right2);
	return{ r1,r2 };
}

bool BattleShipPiece::BattleShipRect::contains(const Point& p)
{
	return area_.first.first <= p.first
		&& area_.second.first >= p.first
		&& area_.first.second <= p.second
		&& area_.second.second >= p.second;
}

bool BattleShipPiece::BattleShipRect::overlaps(const BattleShipRect& r)
{
	pair<Rect,Rect> safe_zone = r.safeZone();
	return (area_.first.second <= safe_zone.first.second.second
		&& area_.second.second >= safe_zone.first.first.second
		&& area_.first.first <= safe_zone.first.second.first
		&& area_.second.first >= safe_zone.first.first.first)
		|| (area_.first.second <= safe_zone.second.second.second
			&& area_.second.second >= safe_zone.second.first.second
			&& area_.first.first <= safe_zone.second.second.first
			&& area_.second.first >= safe_zone.second.first.first);
}

int BattleShipPiece::BattleShipRect::pointToIndex(const Point & p)
{
	if (ori_ == Horizontal) return p.second - area_.first.second;
	return p.first - area_.first.first;
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

AttackResult BattleShipPiece::attack(const Point & p)
{
	if (!rect_.contains(p)) return AttackResult::Miss;
	rect_.hit_[rect_.pointToIndex(p)] = true;
	if (isSunk()) return AttackResult::Sink;
	return AttackResult::Hit;
}

bool BattleShipPiece::overlaps(BattleShipPiece * ship)
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

void BattleShipNavy::addBattleShip(BattleShipPiece * ship)
{
	ships_.push_back(ship);
	sunkships.push_back(false);
	if (ship->player() == 0) aShips++;
	else bShips++;
}

AttackResult BattleShipNavy::attack(const Point & p)
{

	AttackResult res;
	Point p_ = { p.first - 1, p.second - 1 };
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

bool BattleShipNavy::isValid() const
{
	bool error = false;
	// Check navy size (should be exactly 5)
	if (aShips < 5) {
		cout << "Too few ships for player A" << endl;
		error = true;
	}
	if (aShips > 5) {
		cout << "Too many ships for player A" << endl;
		error = true;
	}
	if (bShips < 5) {
		cout << "Too few ships for player B" << endl;
		error = true;
	}
	if (bShips > 5) {
		cout << "Too many ships for player B" << endl;
		error = true;
	}
	
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
	
	if(!error)	return true;
	return false;
}

bool BattleShipNavy::isContinuousShip(const char** board, const Point& p, char m, int model_size, BattleShipPiece::ShipOrientation orientation)
{
	size_t x, y;

	if (orientation == BattleShipPiece::ShipOrientation::Horizontal) {
		for (x = p.second + 1; x < ((size_t)p.second + (size_t)model_size); x++) {
			if (board[p.first][x] != m)
				return false;
		}
	}

	else {
		for (y = p.first + 1; y < ((size_t)p.first + (size_t)model_size); y++) {
			if (board[y][p.second] != m)
				return false;
		}
	}

	return true;
}

bool BattleShipNavy::isOverlappingItself(const char** board, const Point& p, char m, int model_size, BattleShipPiece::ShipOrientation orientation)
{
	size_t x, y;

	/* Check if ship is too big */
	if (orientation == BattleShipPiece::ShipOrientation::Horizontal) {
		if ((p.second > 0 && board[p.first][p.second - 1] == m)
			|| (p.second + model_size < cols_ - -1 && board[p.first][p.second + model_size] == m))
			return true;
	}
	if (orientation == BattleShipPiece::ShipOrientation::Vertical) {
		if ((p.first > 0 && board[p.first - 1][p.second] == m)
			|| (p.first + model_size < rows_ - -1 && board[p.first + model_size][p.second] == m))
			return true;
	}

	/* Check for identical adjacent ship */
	if (orientation == BattleShipPiece::ShipOrientation::Horizontal) {
		for (x = p.second; x < (size_t)p.second + model_size; x++)
		{
			if ((p.first > 0 && board[p.first - 1][x] == m)
				|| (p.first < rows_ - 1 && board[p.first + 1][x] == m))
				return true;
		}
	}

	if (orientation == BattleShipPiece::ShipOrientation::Vertical) {
		for (y = p.first; y < (size_t)p.first + model_size; y++)
		{
			if ((p.second > 0 && board[y][p.second - 1] == m)
				|| (p.second < cols_ - 1 && board[y][p.second + 1] == m))
				return true;
		}
	}

	return false;
}

int BattleShipNavy::isValidShipAt(const char** board, const Point& p, char m)
{
	int player = 1-!islower(m);
	int model_size = BattleShipModel::fromChar(m)->size();
	BattleShipPiece::ShipOrientation orientation;

	// Check for horizontal ship
	if (p.second <= cols_ - model_size && board[p.first][p.second + model_size - 1] == m)
		orientation = BattleShipPiece::ShipOrientation::Horizontal;
	
	// Check for vertical ship
	else if (p.first <= rows_ - model_size && board[p.first + model_size - 1][p.second] == m)
		orientation = BattleShipPiece::ShipOrientation::Vertical;

	else
		return -1;

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