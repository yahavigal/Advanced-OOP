#include "IBattleshipGameAlgoCommon.h"

using namespace std;

enum GameStrategy
{
	HUNTING, EXPLORING, EXPLOITING
};

class Ultimate : public IBattleshipGameAlgoCommon{
	set<Point> boardPoints;
	set<Point> wasHit;
	vector<Point> moves;
	vector<Point> toErase;
	vector<Point> sunkShip;
	GameStrategy strategy;

	bool hitAlready;
	bool shipUnderAttack = false;

	void updateStack();
	void isHorizontal();
	void isVertical();
	void checkWasSunk(const Point &p);
	void checkWasHit();
	void checkWasSunkOnHit(const Point &p);
	void checkWasSunkOntoErase(const Point &p);

	/* Simulation methods */
	vector<Point> simulateShipFromCross(Point p, bool explored_only);
	void expandBounds(vector<Point>& ship_vec, bool horizontal);
	void simulateShipFromVector(vector<Point>& ship_vec);
	vector<Point> simulateShip(Point p, bool explored_only);

	/* Notify handlers */
	void onHit(Point p);
	void onSink(Point p);
	void onOwnHit(Point p);
	void onOwnSink(Point p);

	/* Strategy algorithms */
	void initExploit();
	void pushCross(Point p);
	void pushFromVector(vector<Point> ship_vec);
	void restrategize();
	bool simulateAttack(Point p);
	Point genRandomPoint();

	/* Post-sink handlers */
	void removeCross(const Point& p);
	void removeSunkShip(vector<Point> ship_vec);

public:
	Ultimate(){}
	virtual ~Ultimate(){}

	virtual bool init(const std::string& path) override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;
	virtual std::pair<int, int> attack() override;

	friend IBattleshipGameAlgo* GetAlgorithm();
};

ALGO_API IBattleshipGameAlgo* GetAlgorithm();