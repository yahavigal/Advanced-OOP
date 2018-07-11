#include "../Advanced-OOP-Ex3/Types.h"

enum GameStrategy
{
	HUNTING, EXPLORING, EXPLOITING
};

class SmartAlgo : public IBattleshipGameAlgo
{
	BattleshipBoard		board;
	GameStrategy		strategy;
	set<Coordinate>		boardCoors;
	set<Coordinate>		wasHit;
	vector<Coordinate>	moves;
	vector<Coordinate>	toErase;
	vector<Coordinate>	sunkShip;



public:
	SmartAlgo();
	virtual ~SmartAlgo();
	bool init(const std::string& path);
	void removeCross(const Coordinate &c);
	Coordinate attack();
	void restrategize();
	vector<Coordinate> simulateShipFromCross(Coordinate c, bool explored_only);
	void simulateShipFromVector(vector<Coordinate>& ship_vec);

	virtual void setPlayer(int player) override;
	virtual void setBoard(const BoardData& board) override;
	virtual Coordinate attack() override;
	virtual void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;
};

ALGO_API IBattleshipGameAlgo* GetAlgorithm();