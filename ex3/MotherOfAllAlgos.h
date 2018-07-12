#include "Types.h"
#include "CoordinateSTL.h"

using namespace std;

enum GameStrategy
{
	HUNTING, EXPLORING, EXPLOITING
};

class SmartAlgo : public IBattleshipGameAlgo
{
	//const BoardData*    		Board;
	int					rows, cols, depth;
	GameStrategy		strategy;
	CoordinateSet		orgBoardCoors;
	CoordinateSet		boardCoors;
	CoordinateSet		wasHit;
	CoordinateVector	moves;
	CoordinateVector	toErase;
	CoordinateVector	sunkShip;
	int					Player;
	bool				hitAlready;
	bool				shipUnderAttack = false;


public:
	SmartAlgo() {

	}
	virtual ~SmartAlgo() { }
	//bool init();
	void removeCross(const Coordinate &c);
	void restrategize();
	CoordinateVector simulateShipFromCross(Coordinate c, bool explored_only);
	void simulateShipFromVector(CoordinateVector& ship_vec);
	bool SmartAlgo::addCandidates(CoordinateVector& ship_vec, bool horizontal, bool vertical, bool depthical);
	CoordinateVector simulateShip(Coordinate c, bool explored_only);
	Coordinate genRandomCoor();
	void initExploit();
	void pushCross(Coordinate c);
	void pushFromVector(CoordinateVector ship_vec);
	bool simulateAttack(Coordinate c);
	void updateStack();
	void checkWasHit();
	void checkWasSunk(const Coordinate &cr);
	void checkWasSunkOnHit(const Coordinate &cr);
	void checkWasSunkOntoErase(const Coordinate &cr);
	void isHorizontal();
	void isVertical();
	void isDepthical();

	virtual void setPlayer(int player) override;
	virtual void setBoard(const BoardData& board) override;
	virtual Coordinate attack() override;
	virtual void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;
};

ALGO_API IBattleshipGameAlgo* GetAlgorithm();