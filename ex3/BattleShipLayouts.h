#ifndef BATTLESHIP_LAYOUTS_H
#define BATTLESHIP_LAYOUTS_H

#include "Types.h"
#include "IBattleShipGameAlgo.h"

using namespace std;
using Arr_1D = vector < char>;
using Arr_2D = vector <  Arr_1D>;
using Arr_3D = vector <  Arr_2D>;
/*
* This is a class of multiple singletons,
* each represents a different BattleShip model.
*/
struct BattleShipModel
{
	int length, value;

	static BattleShipModel B() { return{ 1, 2 }; }
	static BattleShipModel P() { return{ 2, 3 }; }
	static BattleShipModel M() { return{ 3, 7 }; }
	static BattleShipModel D() { return{ 4, 8 }; }

	bool operator==(const BattleShipModel & m) { return length == m.length; }

	static vector<BattleShipModel> modelsToArray() {
		return{ B(), P(), M(), D() };
	}

	static BattleShipModel fromChar(char c)
	{
		c = tolower(c);
		switch (c)
		{
		case 'b':
			return B();
		case 'p':
			return P();
		case 'm':
			return M();
		case 'd':
			return D();
		default:
			return{ -1, -1 };
		}
	}
};

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
		Horizontal, Vertical, Depthical
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

								/* Constructor */
		BattleShipRect(const Coordinate & c, BattleShipModel m, ShipOrientation o);

		/*
		* Returns the board area in which other ships are not allowed to exist.
		*/
		tuple<Rect, Rect, Rect> safeZone() const;

		/*
		* Returns TRUE if @p is inside the ship area.
		*/
		bool contains(const Coordinate& p);

		/*
		* Returns TRUE if @r is adjacent to this ship.
		*/
		bool overlaps(const BattleShipRect& r);

		/*
		* Translates @p to its corresponding index in the damage-status vector.
		*/
		int coorToIndex(const Coordinate & p);
	};

public:
	BattleShipRect rect_;		// Geometry subclass
	BattleShipModel model_;
	int player_;

public:
	BattleShipPiece(const Coordinate & c, BattleShipModel m, ShipOrientation o, int player) :rect_(c, m, o), model_(m), player_(player) {}
	~BattleShipPiece() {}

	/*
	* Returns the ship model.
	*/
	BattleShipModel model() const { return model_; }

	/*
	* Returns TRUE if all parts of the ship are hit.
	*/
	bool isSunk() const;

	/*
	* Returns the damage caused by firing at @p.
	*/
	AttackResult attack(const Coordinate & p);

	/*
	* Returns TRUE if this ship and @ship are adjacent.
	*/
	bool overlaps(const shared_ptr<BattleShipPiece> & ship);

	bool contains(const Coordinate& p) { return rect_.contains(p); }

	int player() const { return player_; }
};

/*
* Represents and manages the player's ships.
*/
class BattleShipNavy
{
	int rows_, cols_, depth_;
	vector<shared_ptr<BattleShipPiece>> ships_;
	int aShips, bShips;
	vector<bool> sunkships;
	vector<char> error_shown;

	static bool BattleShipNavy::isContinuousShip(Arr_3D board, const Coordinate& p, char m, int model_size, BattleShipPiece::ShipOrientation orientation);
	bool BattleShipNavy::isOverlappingItself(Arr_3D board, const Coordinate& p, char m, int model_size, BattleShipPiece::ShipOrientation orientation);

public:
	BattleShipNavy(int rows, int cols, int depth) :aShips(0), bShips(0), error_shown(8), rows_(rows), cols_(cols), depth_(depth) {}
	~BattleShipNavy();

	
	/*
	* Add the ship @ship to the navy.
	*/
	void addBattleShip(const shared_ptr<BattleShipPiece> & ship);

	/*
	* Returns the damaged caused by firing at @p.
	*/
	AttackResult attack(const Coordinate & p);

	/*
	* Returns TRUE if the navy is validly constructed.
	*/
	bool isValid() const;

	BattleShipModel modelAt(const Coordinate& p);
	int defeated() const;

	int isValidShipAt(Arr_3D board, const Coordinate& p, char m);

	/*
	* Returns the ships_ vector
	*/
	vector<shared_ptr<BattleShipPiece >> getShipsVec() const;
};

class BattleshipBoard : public BoardData
{

	Arr_3D mBoard;
	BattleShipNavy		mNavy;

public:
	BattleshipBoard(int pRows, int pCols, int pDepth)
		:mNavy(pRows, pCols, pDepth)
	{
		_rows = pRows;
		_cols = pCols;
		_depth = pDepth;
		mBoard = Arr_3D(_rows, Arr_2D(_cols, Arr_1D(_depth)));
	}

	virtual ~BattleshipBoard()
	{
		mBoard.clear();
	}

	virtual char charAt(Coordinate c) const override;
	void put(const Coordinate & c, char ch);
	void parseBoardLine(int depth, int row, const string& line);
	tuple<string, string> buildNavy();
	BattleShipNavy& getNavy() { return mNavy; }
};
#endif