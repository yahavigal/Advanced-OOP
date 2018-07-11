#include "BattleShipLayouts.h"

using namespace std;

// ***************************** //
//   BattleShipModel Singelton   //
// ***************************** //

BattleShipModel * modelB()
{
	static BattleShipModel * b = new BattleShipModel(1, 2);
	return b;
}

BattleShipModel * modelP()
{
	static BattleShipModel * p = new BattleShipModel(2, 3);
	return p;
}

BattleShipModel * modelM()
{
	static BattleShipModel * m = new BattleShipModel(3, 7);
	return m;
}

BattleShipModel * modelD()
{
	static BattleShipModel * d = new BattleShipModel(4, 8);
	return d;
}

vector<BattleShipModel *> modelsToArray()
{
	static vector<BattleShipModel *> vec = { modelB(), modelP(), modelM(), modelD() };
	return vec;
}

BattleShipModel * fromChar(char c)
{
	c = tolower(c);
	switch (c)
	{
	case 'b':
		return modelB();
	case 'p':
		return modelP();
	case 'm':
		return modelM();
	case 'd':
		return modelD();
	default:
		return nullptr;
	}
}

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
	for (size_t i = 0; i < ships_.size(); i++)
	{
		if (sunkships[i]) continue;
		res = ships_[i]->attack(p);
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
				cout << "Adjacent ships on board" << endl;
				return false;
			}
		}
	}
	

	if(!error)	return true;
	return false;
}

int BattleShipNavy::isValidShipAt(const Board& board, const Point& p, char m)
{
	size_t x, y;
	int suspect = 0;
	int player = 1-!islower(m);
	BattleShipModel * model = fromChar(m);
	int model_size = model->size();

	// Check for horizontal ship
	if (p.second <= (int)board[0].size() - model_size && (board[p.first][p.second + model_size - 1] == m))
	{
		for (x = p.second; x < ((size_t)p.second + (size_t)model_size); x++)
		{

			if (board[p.first][x] != m) {
				suspect = 1;
				break;
			}
		}
		if (suspect == 0) {
			if (x == p.second + model_size) {
				if (p.second > 0) {
					if (board[p.first][p.second - 1] == m) { //this ship is too big
						if (!error_shown[4 * player + model_size - 1]) {
							cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
							error_shown[4 * player + model_size - 1] = true;
						}

						return -1;
					}
				}
				if (p.second + model_size < 9) {
					if (board[p.first][p.second + model_size] == m) { //this ship is too big

						if (!error_shown[4 * player + model_size - 1]) {
							cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
							error_shown[4 * player + model_size - 1] = true;
						}

						return -1;
					}
				}
			}

			//overlaping from the same kind
			if (p.first > 0) {
				for (x = p.second; x < (size_t)p.second + model_size; x++)
				{
					if (board[p.first - 1][x] == m) {
						if (!error_shown[4 * player + model_size - 1]) {
							cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
							error_shown[4 * player + model_size - 1] = true;
						}

						return -1;
					}
				}
			}
			if (p.first < 9) {
				for (y = p.second; y < (size_t)p.second + model_size; y++)
				{
					if (board[p.first + 1][y] == m) {
						if (!error_shown[4 * player + model_size - 1]) {
							cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
							error_shown[4 * player + model_size - 1] = true;
						}

						return -1;
					}
				}
			}
			return BattleShipPiece::ShipOrientation::Horizontal;
		}
	}
	// Check for vertical ship
	if (p.first <= (int)(board.size() - model_size) && board[p.first + model_size - 1][p.second] == m)
	{
		for (y = p.first; y <(size_t) p.first + model_size; y++)
		{
			if (board[y][p.second] != m) {
				if (!error_shown[4 * player + model_size - 1]) {
					cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
					error_shown[4 * player + model_size - 1] = true;
				}
				
				return -1;
			}
		}
		if (y == p.first + model_size) {
			if (p.first > 0) {
				if (board[p.first-1][p.second ] == m) { //this ship is too big
					if (!error_shown[4 * player + model_size - 1]) {
						cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
						error_shown[4 * player + model_size - 1] = true;
					}

					return -1;
				}
			}
			if (p.first+ model_size < 9) {
				if (board[p.first+ model_size][p.second ] == m) { //this ship is too big
					if (!error_shown[4 * player + model_size - 1]) {
						cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
						error_shown[4 * player + model_size - 1] = true;
					}

					return -1;
				}
			}
		}
		//overlaping from the same kind
		if (p.second > 0) {
			for (y = p.first; y <(size_t)p.first + model_size; y++)
			{
				if (board[y][p.second-1] == m) {
					if (!error_shown[4 * player + model_size - 1]) {
						cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
						error_shown[4 * player + model_size - 1] = true;
					}

					return -1;
				}
			}
		}
		if (p.second <9) {
			for (y = p.first; y <(size_t)p.first + model_size; y++) 
			{
				if (board[y][p.second + 1] == m) {
					if (!error_shown[4 * player + model_size - 1]) {
						cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
						error_shown[4 * player + model_size - 1] = true;
					}

					return -1;
				}
			}
		}
		return BattleShipPiece::ShipOrientation::Vertical;
	}
	if (((p.first > 0) && (board[p.first - 1][p.second] == ' '))&& ((p.second > 0) && (board[p.first][p.second - 1] == ' '))) {
		if (!error_shown[4 * player + model_size - 1]) {
				cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
				error_shown[4 * player + model_size - 1] = true;
			}

			return -1;
		}
	else {
		if (((p.first > 0) && (board[p.first - 1][p.second] == ' ')&&(p.second==0))){
			if (!error_shown[4 * player + model_size - 1]) {
				cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
				error_shown[4 * player + model_size - 1] = true;
			}

			return -1;
		}
		else if (((p.first == 0) && (p.second > 0) &&(board[p.first ][p.second-1] == ' ') )) {
			if (!error_shown[4 * player + model_size - 1]) {
				cout << "Wrong size or shape for ship " << m << " for player " << (player ? "B" : "A") << endl;
				error_shown[4 * player + model_size - 1] = true;
			}

			return -1;
		}
		
	}


	return -1;	// shouldn't reach this.
}