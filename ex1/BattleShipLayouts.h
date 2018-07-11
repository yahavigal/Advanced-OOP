#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <map>
#include "IBattleShipGameAlgo.h"

using namespace std;

typedef vector<vector<char>> Board;
typedef pair<int, int> Point;
typedef pair<Point, Point> Rect;


/*
* This is a class of multiple singletons,
* each represents a different BattleShip model.
*/
class BattleShipModel
{
	int size_;		// No. of cells occupied by ship.
	int value_;		// No. of points earned by sinking ship.	

	BattleShipModel(int size, int value) :size_(size), value_(value) {}

public:
	int size() const { return size_; }
	int value() const { return value_; }

	BattleShipModel(const BattleShipModel& m) = delete;
	BattleShipModel& operator=(const BattleShipModel& m) = delete;
	~BattleShipModel() {}

	friend BattleShipModel * modelB();
	friend BattleShipModel * modelP();
	friend BattleShipModel * modelM();
	friend BattleShipModel * modelD();
};

BattleShipModel * modelB();
BattleShipModel * modelP();
BattleShipModel * modelM();
BattleShipModel * modelD();
vector<BattleShipModel *> modelsToArray();
BattleShipModel * fromChar(char c);

/*
* Represents a game piece and tracks its damage.
*/
class BattleShipPiece
{
public:
	/*
	* Represents the ship's orientation.
	*/
	enum ShipOrientation
	{
		Horizontal, Vertical
	};

private:
	/*
	* This geometry subclass contains functions helpful for 2d-mapping the ship across the board.
	*/
	struct BattleShipRect
	{
		Rect area_;				// Area of the ship on the board
		ShipOrientation ori_;	// Ship orientation
		vector<bool> hit_;		// Damage status of ship
								/*
								* Constructor
								*/
		BattleShipRect(const Point & p, BattleShipModel * m, ShipOrientation o);

		/*
		* Returns the board area in which other ships are not allowed to exist.
		*/
		pair<Rect,Rect> safeZone() const;

		/*
		* Returns TRUE if @p is inside the ship area.
		*/
		bool contains(const Point& p);

		/*
		* Returns TRUE if @r is adjacent to this ship.
		*/
		bool overlaps(const BattleShipRect& r);

		/*
		* Translates @p to its corresponding index in the damage-status vector.
		*/
		int pointToIndex(const Point & p);
	};
public:
	BattleShipRect rect_;		// Geometry subclass
	BattleShipModel * model_;
	int player_;

public:
	BattleShipPiece(const Point & p, BattleShipModel * m, ShipOrientation o, int player) :rect_(p, m, o), model_(m), player_(player){}
	~BattleShipPiece() {}

	/*
	* Returns the ship model.
	*/
	BattleShipModel * model() const { return model_; }

	/*
	* Returns TRUE if all parts of the ship are hit.
	*/
	bool isSunk() const;

	/*
	* Returns the damage caused by firing at @p.
	*/
	AttackResult attack(const Point & p);

	/*
	* Returns TRUE if this ship and @ship are adjacent.
	*/
	bool overlaps(BattleShipPiece * ship);

	bool contains(const Point& p) { return rect_.contains(p); }

	int player() const { return player_; }
};

/*
* Represents and manages the player's ships.
*/
class BattleShipNavy
{
	vector<BattleShipPiece *> ships_;
	int aShips, bShips;
	vector<bool> sunkships;
	vector<char> error_shown;

public:
	BattleShipNavy():aShips(0),bShips(0),error_shown(8) {}
	~BattleShipNavy();

	/*
	* Add the ship @ship to the navy.
	*/
	void addBattleShip(BattleShipPiece * ship);

	/*
	* Returns the damaged caused by firing at @p.
	*/
	AttackResult attack(const Point & p);

	/*
	* Returns TRUE if the navy is validly constructed.
	*/
	bool isValid() const;

	BattleShipModel* modelAt(const Point& p)
	{
		for (size_t t = 0; t < ships_.size(); t++){
			if (ships_[t]->contains(p))
				return ships_[t]->model();
		}
		return NULL;
	}

	int defeated()
	{
		if (aShips == 0) return 1;
		if (bShips == 0) return 0;
		return -1;
	}

	int isValidShipAt(const Board& board, const Point& p, char m);
};
