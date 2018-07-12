#include "Ultimate.h"

using namespace std;

bool Ultimate::init(const std::string& path)
{
	/* Initialize data members */
	strategy = HUNTING;
	for (int i = 1; i <= rows; i++){
		for (int j = 1; j <= cols; j++)
			boardPoints.insert({ i, j });
	}

	/* Eliminate points from set */
	for (int i = 1; i <= rows; i++){
		for (int j = 1; j <= cols; j++){
			set<Point>::iterator it = boardPoints.find({ i, j });
			if ((matrix[i - 1][j - 1] != ' ')){
				if (it != boardPoints.end())
					boardPoints.erase(it);
				removeCross({ i, j });
			}
		}
	}
	return true;
}



std::pair<int, int> Ultimate::attack()
{
	/* Return a no-move if needed (should never reach this) */
	if (boardPoints.empty())
		return{ -1, -1 };

	Point pt;
	restrategize();

	/* Hunting mode - shoot at random */
	if (strategy == HUNTING)
		return genRandomPoint();

	/* Exploiting mode - use opponent's own hits against them */
	if (strategy == EXPLOITING)
		initExploit();

	/* Exploiting/Exploring mode - shoot around a discovered ship */
	pt = moves.back();
	moves.pop_back();

	/* Check if a simulated hit on 'pt' creates an illegaly-structed ship */
	if (!simulateAttack(pt))
		return attack();

	return pt;
}



void Ultimate::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (boardPoints.empty()) {
		return;
	}
	set<Point>::iterator it = boardPoints.find({ row, col });
	Point pnt = { row, col };
	if (it != boardPoints.end())
		boardPoints.erase(it); // erase the point from our board 
	if (Player != player) { // if the notify was due to the other players' turn 
		vector<Point>::iterator mit = find(moves.begin(),moves.end(),pnt);
		if (result == AttackResult::Miss) { // if he missed
			if (mit != moves.end()){ // if that point was on our "next move" set erase it
				moves.erase(mit);
			}
		}
		else if (matrix[row - 1][col - 1] == ' '){ // if the other player did own gol
			set<Point>::iterator it = boardPoints.find({ row, col });
			if (result == AttackResult::Hit){
				wasHit.insert({ row, col }); // save the point for later - when we attack the same ship
				if (mit != moves.end()) // if that point was on our "next move" set we want to update it
					updateStack();
			}
			else { // if the other player sunk it's own ship
				sunkShip.push_back({ row, col });
				checkWasSunk({ row, col }); // find all Points from the same ship 
				for (Point pt : sunkShip){ // delete it
					set<Point>::iterator it = boardPoints.find(pt);
					removeCross(pt);
					if (it != boardPoints.end())
						boardPoints.erase(it);
				}
				sunkShip.clear();
				if (shipUnderAttack){
					shipUnderAttack = false;
					moves.clear();
					toErase.clear();
					hitAlready = false;
				}
			}
		}
	}
	else {
		if (result == AttackResult::Hit) {
			toErase = simulateShip(pnt, false);
			if (toErase.size() == 1)
				pushCross({ row, col });
			else
				pushFromVector(toErase);
		}
		else if (result == AttackResult::Sink) {
			toErase = simulateShip(pnt, false);
			if (toErase.size() == 1) {
				removeCross(pnt);
			}
			else {
				hitAlready = false; 
				moves.clear();
				for (Point pt : toErase) {
					set<Point>::iterator it = boardPoints.find(pt);
					removeCross(pt);
					if (it != boardPoints.end()){
						boardPoints.erase(it);
					}
					it = wasHit.find(pt);
					if (it != wasHit.end()){
						wasHit.erase(it);
					}
				}
			}
			toErase.clear();
			set<Point>::iterator it = boardPoints.find({ row, col });
			if (it!=boardPoints.end())
				boardPoints.erase(it);
		}
		else
		{
			set<Point>::iterator it = boardPoints.find({ row, col });
			if (it != boardPoints.end())
				boardPoints.erase(it);
		}
	}
}

void Ultimate::removeCross(const Point &p) {
	vector<Point> points = { { p.first - 1, p.second },{ p.first + 1, p.second },{ p.first, p.second - 1 },{ p.first, p.second + 1 } };
	for (Point& pt : points) {
		set<Point>::iterator it = boardPoints.find(pt);
		if (it != boardPoints.end()) {
			boardPoints.erase(it);
		}
	}
}

void Ultimate::updateStack(){
	checkWasHit();
	if (toErase.size() > 1){
		moves.clear();
		if (toErase[0].first == toErase[1].first)
			isHorizontal();
		else
			isVertical();
	}
}

void Ultimate::checkWasHit(){
	bool hit = false;
	for (Point pt : moves){
		set<Point>::iterator it;
		if ((it = wasHit.find(pt)) != wasHit.end()){
			toErase.push_back(pt);
			wasHit.erase(it);
			hitAlready = true;
			moves.clear();
			if (toErase[0].first == toErase[1].first){
				isHorizontal();
			}
			else{
				isVertical();
			}
			hit = true;
			break;
		}
	}

	if (hit){
		checkWasHit();
	}
}


void Ultimate::checkWasSunkOnHit(const Point &p){
	if (!wasHit.empty()){
		vector<Point> points = { { p.first - 1, p.second }, { p.first + 1, p.second }, { p.first, p.second - 1 }, { p.first, p.second + 1 } };
		for (Point& pt : points) {
			set<Point>::iterator it = wasHit.find(pt);
			if (it != wasHit.end()) { // if one of the cross points is at "wasHit"
				sunkShip.push_back(pt);
				wasHit.erase(it);
				checkWasSunk(pt);
			}
		}
	}
}

void Ultimate::checkWasSunkOntoErase(const Point &p){
	if (!toErase.empty()){
		vector<Point> points = { { p.first - 1, p.second }, { p.first + 1, p.second }, { p.first, p.second - 1 }, { p.first, p.second + 1 } };
		for (Point& pt : points) {
			vector<Point>::iterator it = find(toErase.begin(), toErase.end(), pt);
			if (it != toErase.end()) { // if one of the cross points is at "toErase"
				shipUnderAttack = true;
				sunkShip.push_back(pt);
				toErase.erase(it);
				checkWasSunk(pt);
			}
		}
	}
}

void Ultimate::checkWasSunk(const Point &p){
	checkWasSunkOnHit(p);
	checkWasSunkOntoErase(p);
}

void Ultimate::isHorizontal(){
	sort(toErase.begin(), toErase.end(), [](Point& p1, Point& p2){return p1.second < p2.second; });
	Point p1 = { toErase[0].first, toErase[0].second - 1 };
	if (wasHit.find(p1) != wasHit.end()){ // if the point was already hit by the second player - update toErase and go again
		toErase.push_back(p1);
		wasHit.erase(wasHit.find(p1));
		isHorizontal();
	}
	Point p2 = { toErase[0].first, toErase[toErase.size() - 1].second + 1 };
	if (wasHit.find(p2) != wasHit.end()){ // if the point was already hit by the second player - update toErase and go again
		toErase.push_back(p2);
		wasHit.erase(wasHit.find(p2));
		isHorizontal();
	}
	// if the point is still on the board - add it to the "next move" set
	if (boardPoints.find(p1) != boardPoints.end()){
		moves.push_back(p1);
	}
	if (boardPoints.find(p2) != boardPoints.end()){
		moves.push_back(p2);
	}
}

void Ultimate::isVertical(){
	sort(toErase.begin(), toErase.end(), [](Point& p1, Point& p2){return p1.first < p2.first; });
	Point p1 = { toErase[0].first - 1, toErase[0].second };
	if (wasHit.find(p1) != wasHit.end()){ // if the point was already hit by the second player - update toErase and go again
		toErase.push_back(p1);
		wasHit.erase(wasHit.find(p1));
		isVertical();
	}
	Point p2 = { toErase[toErase.size() - 1].first + 1, toErase[0].second };
	if (wasHit.find(p2) != wasHit.end()){ // if the point was already hit by the second player - update toErase and go again
		toErase.push_back(p2);
		wasHit.erase(wasHit.find(p2));
		isVertical();
	}
	// if the point is still on the board - add it to the "next move" set
	if (boardPoints.find(p1) != boardPoints.end()){
		moves.push_back(p1);
	}
	if (boardPoints.find(p2) != boardPoints.end()){
		moves.push_back(p2);
	}
}

Point Ultimate::genRandomPoint()
{
	random_device rd;
	default_random_engine dre(rd());

	set<Point>::iterator it = boardPoints.begin();
	advance(it, dre()%boardPoints.size());

	return *it;
}

bool Ultimate::simulateAttack(Point p)
{
	vector<Point> simulated_ship = simulateShip(p, false);

	/* Check if simulated ship is of legal size */
	if (simulated_ship.size() > 4 || simulated_ship.empty())
	{
		if (boardPoints.find(p)!=boardPoints.end()) boardPoints.erase(boardPoints.find(p));
		return false;
	}

	return true;
}

void Ultimate::restrategize()
{
	if (!moves.empty())
		strategy = EXPLORING;

	else if (!wasHit.empty())
		strategy = EXPLOITING;

	else
		strategy = HUNTING;
}

void Ultimate::pushCross(Point p)
{
	int x[4] = { -1, +1, 0, 0 };
	int y[4] = { 0, 0, -1, +1 };
	for (size_t t = 0; t < 4; t++)
	{
		Point tmp_pt = { p.first + x[t], p.second + y[t] };
		if (boardPoints.find(tmp_pt) != boardPoints.end())
			moves.push_back(tmp_pt);
	}
}

void Ultimate::pushFromVector(vector<Point> ship_vec)
{
	moves.clear();
	Point cand_pt1, cand_pt2;
	if (ship_vec[0].first == ship_vec[1].first)
	{
		cand_pt1 = { ship_vec[0].first, ship_vec[0].second - 1 };
		cand_pt2 = { ship_vec[0].first, ship_vec[ship_vec.size() - 1].second + 1 };
	}
	else
	{
		cand_pt1 = { ship_vec[0].first - 1, ship_vec[0].second };
		cand_pt2 = { ship_vec[ship_vec.size() - 1].first + 1, ship_vec[0].second };
	}

	if (boardPoints.find(cand_pt1) != boardPoints.end())
		moves.push_back(cand_pt1);

	if (boardPoints.find(cand_pt2) != boardPoints.end())
		moves.push_back(cand_pt2);
}

void Ultimate::initExploit()
{
	/* Exploit a random ship */
	set<Point>::iterator target_it = wasHit.begin();
	Point target = *target_it;
	toErase.push_back(target);
	wasHit.erase(target_it);

	/* Create virtual ship around the chosen point */
	vector<Point> simulated = simulateShip(target, false);

	if (simulated.size() == 1)
		pushCross(target);
	else
		pushFromVector(simulated);
}

vector<Point> Ultimate::simulateShipFromCross(Point p, bool explored_only)
{
	vector<Point> simulated_ship = { p };

	/* Find hits in cross */
	int x[4] = { -1, +1, 0, 0 };
	int y[4] = { 0, 0, -1, +1 };
	for (size_t t = 0; t < 4; t++)
	{
		Point tmp_pt = { p.first + x[t], p.second + y[t] };
		if ((!explored_only && wasHit.find(tmp_pt) != wasHit.end()) || find(toErase.begin(), toErase.end(), tmp_pt) != toErase.end())
		{
			for (auto pt : simulated_ship)
			{
				/* Invalid ship structure */
				if (pt.first != tmp_pt.first && pt.second != tmp_pt.second)
					return{};
			}

			simulated_ship.push_back(tmp_pt);
		}
	}

	return simulated_ship;
}

void Ultimate::simulateShipFromVector(vector<Point>& ship_vec)
{
	while (true)
	{
		bool horizontal;
		std::sort(ship_vec.begin(), ship_vec.end(), [&horizontal](Point p1, Point p2) {
			if (p1.first == p2.first)
			{
				horizontal = true;
				return p1.second < p2.second;
			}
			horizontal = false;
			return p1.first < p2.first;
		});

		Point cand_pt1, cand_pt2;
		if (horizontal)
		{
			cand_pt1 = { ship_vec[0].first, ship_vec[0].second - 1 };
			cand_pt2 = { ship_vec[0].first, ship_vec[ship_vec.size() - 1].second + 1 };
		}
		else
		{
			cand_pt1 = { ship_vec[0].first - 1, ship_vec[0].second };
			cand_pt2 = { ship_vec[ship_vec.size() - 1].first + 1, ship_vec[0].second };
		}
		if (wasHit.find(cand_pt1) != wasHit.end() || find(toErase.begin(), toErase.end(), cand_pt1) != toErase.end())
		{
			ship_vec.push_back(cand_pt1);
			continue;
		}
		if (wasHit.find(cand_pt2) != wasHit.end() || find(toErase.begin(), toErase.end(), cand_pt2) != toErase.end())
		{
			ship_vec.push_back(cand_pt2);
			continue;
		}

		break;
	}
}

vector<Point> Ultimate::simulateShip(Point p, bool explored_only)
{
	vector<Point> simulated_ship = simulateShipFromCross(p, explored_only);
	if (simulated_ship.empty())
		return{};

	simulateShipFromVector(simulated_ship);
	return simulated_ship;
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new Ultimate();
}