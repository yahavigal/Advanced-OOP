// MotherOfAllAlgos.cpp : Defines the exported functions for the DLL application.
//

#include "MotherOfAllAlgos.h"

using namespace std;

void SmartAlgo::setBoard(const BoardData&board)
{
	/* Initialize data members */
	strategy = HUNTING;
	rows = board.rows();
	cols = board.cols();
	depth = board.depth();
	boardCoors.fill(rows, cols, depth);

	/* Eliminate points from set */
	for (int i = 1; i <= rows; i++) {
		for (int j = 1; j <= cols; j++) {
			for (int k = 1; k <= depth; k++) {
				if ((board.charAt({ i, j, k }) != ' ')) {
					Coordinate c = { i, j, k };
					orgBoardCoors.insert(c);
					boardCoors.remove(c);
					removeCross(c);
				}
			}
		}
	}
	return;
}

void SmartAlgo::setPlayer(int player)
{
	Player = player;
}

Coordinate SmartAlgo::attack()
{
	/* Return a no-move if needed (should never reach this) */
	if (boardCoors.empty())
		return{ -1, -1, -1 };

	Coordinate cr(0, 0, 0);
	restrategize();

	/* Hunting mode - shoot at random */
	if (strategy == HUNTING)
		return genRandomCoor();

	/* Exploiting mode - use opponent's own hits against them */
	if (strategy == EXPLOITING)
		initExploit();

	/* Exploiting/Exploring mode - shoot around a discovered ship */
	cr = moves.back();
	moves.pop_back();

	/* Check if a simulated hit on 'cr' creates an illegaly-structed ship */

	if (!simulateAttack(cr))
	{
		return attack();
	}

	return cr;
}

void SmartAlgo::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	if (boardCoors.empty()) {
		return;
	}
	boardCoors.remove(move);
	if (Player != player) { // if the notify was due to the other players' turn 
		if (result == AttackResult::Miss) { // if he missed
			moves.erase(move);
		}
		else if (!orgBoardCoors.contains(move)) { // if the other player did own gol
			if (result == AttackResult::Hit) {
				wasHit.insert(move); // save the point for later - when we attack the same ship
				if (moves.contains(move)) // if that point was on our "next move" set we want to update it
					updateStack();
			}
			else { // if the other player sunk it's own ship
				sunkShip.push_back(move);
				checkWasSunk(move); // find all Points from the same ship 
				for (size_t i = 0; i < sunkShip.size(); i++) { // delete it
					Coordinate cr = sunkShip[i];
					removeCross(cr);
					boardCoors.remove(cr);
				}
				sunkShip.clear();
				if (shipUnderAttack) {
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
			toErase = simulateShip(move, false);
			if (toErase.size() == 1)
				pushCross(move);
			else
				pushFromVector(toErase);
		}
		else if (result == AttackResult::Sink) {
			toErase = simulateShip(move, false);
			if (toErase.size() == 1) {
				removeCross(move);
			}
			else {
				hitAlready = false;
				moves.clear();
				for (size_t i = 0; i < toErase.size(); i++) {
					Coordinate cr = toErase[i];
					removeCross(cr);
					boardCoors.remove(cr);
					wasHit.remove(cr);
				}
			}
			toErase.clear();
			boardCoors.remove(move);
		}
		else
		{
			boardCoors.remove(move);
		}
	}
}

void SmartAlgo::removeCross(const Coordinate &c)
{
	vector<Coordinate> coors = { { c.row - 1, c.col, c.depth },{ c.row + 1, c.col, c.depth },{ c.row, c.col - 1, c.depth },{ c.row, c.col + 1, c.depth },{ c.row, c.col, c.depth - 1 },{ c.row, c.col, c.depth + 1 } };
	for (Coordinate& cr : coors) {
		boardCoors.remove(cr);
	}
}

void SmartAlgo::restrategize()
{
	if (!moves.empty())
		strategy = EXPLORING;

	else if (!wasHit.empty())
		strategy = EXPLOITING;

	else
		strategy = HUNTING;
}

CoordinateVector SmartAlgo::simulateShipFromCross(Coordinate c, bool explored_only)
{
	CoordinateVector simulated_ship;
	simulated_ship.push_back(c);

	/* Find hits in cross */
	int x[6] = { -1, +1, 0, 0, 0, 0 };
	int y[6] = { 0, 0, -1, +1, 0, 0 };
	int z[6] = { 0, 0, 0, 0, -1, +1 };
	for (size_t t = 0; t < 6; t++)
	{
		Coordinate tmp_cr = { c.row + x[t], c.col + y[t], c.depth + z[t] };
		if ((!explored_only && wasHit.contains(tmp_cr)) || toErase.contains(tmp_cr))
		{
			for (size_t i = 0; i < simulated_ship.size(); i++)
			{
				Coordinate cr = simulated_ship[i];
				/* Invalid ship structure */
				if (cr.row != tmp_cr.row && cr.col != tmp_cr.col && cr.depth != tmp_cr.depth)
					return CoordinateVector();
			}

			simulated_ship.push_back(tmp_cr);
		}
	}

	return simulated_ship;
}

void SmartAlgo::simulateShipFromVector(CoordinateVector& ship_vec)
{
	while (true)
	{
		bool horizontal = false, vertical = false, depthical = false;
		std::sort(ship_vec.begin(), ship_vec.end(), [&horizontal, &vertical, &depthical](TupledCoordinate tc1, TupledCoordinate tc2)
		{
			Coordinate c1 = CoordinateVector::coordinated(tc1), c2 = CoordinateVector::coordinated(tc2);
			if (c1.row == c2.row && c1.depth == c2.depth)
			{
				horizontal = true;
				return c1.col < c2.col;
			}
			else if (c1.col == c2.col && c1.depth == c2.depth)
			{
				vertical = true;
				return c1.row < c2.row;
			}
			else
			{
				depthical = true;
				return c1.depth < c2.depth;
			}
		});
		if (addCandidates(ship_vec, horizontal, vertical, depthical))
			continue;
		break;
	}
}

bool SmartAlgo::addCandidates(CoordinateVector& ship_vec, bool horizontal, bool vertical, bool depthical)
{
	Coordinate cand_cr1(0, 0, 0), cand_cr2(0, 0, 0);
	if (horizontal)
	{
		cand_cr1 = ship_vec[0];
		cand_cr1.col -= 1;
		cand_cr2 = ship_vec[ship_vec.size() - 1];
		cand_cr2.col += 1;
	}
	else if (vertical)
	{
		cand_cr1 = ship_vec[0];
		cand_cr1.row -= 1;
		cand_cr2 = ship_vec[ship_vec.size() - 1];
		cand_cr2.row += 1;
	}
	else
	{
		cand_cr1 = ship_vec[0];
		cand_cr1.depth -= 1;
		cand_cr2 = ship_vec[ship_vec.size() - 1];
		cand_cr2.depth += 1;
	}
	if (wasHit.contains(cand_cr1) || toErase.contains(cand_cr1))
	{
		ship_vec.push_back(cand_cr1);
		return true;
	}
	if (wasHit.contains(cand_cr2) || toErase.contains(cand_cr2))
	{
		ship_vec.push_back(cand_cr2);
		return true;
	}
	return false;
}

CoordinateVector SmartAlgo::simulateShip(Coordinate c, bool explored_only) {
	CoordinateVector simulated_ship = simulateShipFromCross(c, explored_only);
	if (simulated_ship.empty())
		return CoordinateVector();

	simulateShipFromVector(simulated_ship);
	return simulated_ship;
}

Coordinate SmartAlgo::genRandomCoor()
{
	return boardCoors.random();
}

void SmartAlgo::initExploit()
{
	/* Exploit a random ship */
	Coordinate target = wasHit.first();
	toErase.push_back(target);
	wasHit.remove(target);

	/* Create virtual ship around the chosen point */
	CoordinateVector simulated = simulateShip(target, false);

	if (simulated.size() == 1)
		pushCross(target);
	else
		pushFromVector(simulated);
}

void SmartAlgo::pushCross(Coordinate c)
{
	int x[6] = { -1, +1, 0, 0, 0, 0 };
	int y[6] = { 0, 0, -1, +1, 0, 0 };
	int z[6] = { 0, 0, 0, 0, -1, +1 };
	for (size_t t = 0; t < 6; t++)
	{
		Coordinate tmp_cr = { c.row + x[t], c.col + y[t], c.depth + z[t] };
		if (boardCoors.contains(tmp_cr))
			moves.push_back(tmp_cr);
	}
}

void SmartAlgo::pushFromVector(CoordinateVector ship_vec)
{
	moves.clear();
	Coordinate cand_cr1(0, 0, 0), cand_cr2(0, 0, 0);
	if (ship_vec[0].row == ship_vec[1].row && ship_vec[0].depth == ship_vec[1].depth)
	{
		cand_cr1 = { ship_vec[0].row, ship_vec[0].col - 1, ship_vec[0].depth };
		cand_cr2 = { ship_vec[0].row, ship_vec[ship_vec.size() - 1].col + 1, ship_vec[0].depth };
	}
	else if (ship_vec[0].col == ship_vec[1].col && ship_vec[0].depth == ship_vec[1].depth)
	{
		cand_cr1 = { ship_vec[0].row - 1, ship_vec[0].col, ship_vec[0].depth };
		cand_cr2 = { ship_vec[ship_vec.size() - 1].row + 1, ship_vec[0].col, ship_vec[0].depth };
	}
	else
	{
		cand_cr1 = { ship_vec[0].row, ship_vec[0].col, ship_vec[0].depth - 1 };
		cand_cr2 = { ship_vec[0].row, ship_vec[0].col, ship_vec[ship_vec.size() - 1].depth + 1 };
	}

	if (boardCoors.contains(cand_cr1))
		moves.push_back(cand_cr1);

	if (boardCoors.contains(cand_cr2))
		moves.push_back(cand_cr2);
}

bool SmartAlgo::simulateAttack(Coordinate c)
{
	CoordinateVector simulated_ship = simulateShip(c, false);

	/* Check if simulated ship is of legal size */
	if (simulated_ship.size() > 4 || simulated_ship.empty())
	{
		boardCoors.remove(c);
		return false;
	}

	return true;
}

void SmartAlgo::updateStack() {
	checkWasHit();
	if (toErase.size() > 1) {
		moves.clear();
		if (toErase[0].row == toErase[1].row)
			isHorizontal();
		else if (toErase[0].col == toErase[1].col)
			isVertical();
		else
			isDepthical();
	}
}

void SmartAlgo::checkWasHit() {
	bool hit = false;
	for (size_t i = 0; i < moves.size(); i++) {
		Coordinate cr = moves[i];
		if (wasHit.contains(cr)) {
			toErase.push_back(cr);
			wasHit.remove(cr);
			hitAlready = true;
			moves.clear();
			if (toErase[0].row == toErase[1].row)
				isHorizontal();
			else if (toErase[0].col == toErase[1].col)
				isVertical();
			else
				isDepthical();
			hit = true;
			break;
		}
	}

	if (hit) {
		checkWasHit();
	}
}

void SmartAlgo::checkWasSunk(const Coordinate &cr) {
	checkWasSunkOnHit(cr);
	checkWasSunkOntoErase(cr);
}

void SmartAlgo::checkWasSunkOnHit(const Coordinate &cr) {
	if (!wasHit.empty()) {
		vector<Coordinate> coordinates = { { cr.row - 1, cr.col, cr.depth },{ cr.row + 1, cr.col, cr.depth },{ cr.row, cr.col - 1, cr.depth },{ cr.row, cr.col + 1, cr.depth },{ cr.row, cr.col, cr.depth - 1 },{ cr.row, cr.col, cr.depth + 1 } };
		for (Coordinate& coor : coordinates) {
			if (wasHit.contains(coor))
			{
				sunkShip.push_back(coor);
				wasHit.remove(coor);
				checkWasSunk(coor);
			}
		}
	}
}

void SmartAlgo::checkWasSunkOntoErase(const Coordinate &cr) {
	if (!toErase.empty()) {
		vector<Coordinate> coordinates = { { cr.row - 1, cr.col, cr.depth },{ cr.row + 1, cr.col, cr.depth },{ cr.row, cr.col - 1, cr.depth },{ cr.row, cr.col + 1, cr.depth },{ cr.row, cr.col, cr.depth - 1 },{ cr.row, cr.col, cr.depth + 1 } };
		for (Coordinate& coor : coordinates) {
			if (toErase.contains(coor)) { // if one of the cross coordinates is at "toErase"
				shipUnderAttack = true;
				sunkShip.push_back(coor);
				toErase.erase(coor);
				checkWasSunk(coor);
			}
		}
	}
}

void SmartAlgo::isHorizontal() {
	sort(toErase.begin(), toErase.end(), [](TupledCoordinate& cr1, TupledCoordinate& cr2) {return CoordinateVector::coordinated(cr1).col < CoordinateVector::coordinated(cr2).col; });
	Coordinate cr1 = { toErase[0].row, toErase[0].col - 1, toErase[0].depth };
	if (wasHit.contains(cr1)) { // if the coordinate was already hit by the second player - update toErase and go again
		toErase.push_back(cr1);
		wasHit.remove(cr1);
		isHorizontal();
	}
	Coordinate cr2 = { toErase[0].row, toErase[toErase.size() - 1].col + 1, toErase[0].depth };
	if (wasHit.contains(cr2)) { // if the coordinate was already hit by the second player - update toErase and go again
		toErase.push_back(cr2);
		wasHit.remove(cr2);
		isHorizontal();
	}
	// if the point is still on the board - add it to the "next move" set
	if (boardCoors.contains(cr1)) {
		moves.push_back(cr1);
	}
	if (boardCoors.contains(cr2)) {
		moves.push_back(cr2);
	}
}

void SmartAlgo::isVertical() {
	sort(toErase.begin(), toErase.end(), [](TupledCoordinate& cr1, TupledCoordinate& cr2) {return CoordinateVector::coordinated(cr1).row < CoordinateVector::coordinated(cr2).row; });
	Coordinate cr1 = { toErase[0].row - 1, toErase[0].col, toErase[0].depth };
	if (wasHit.contains(cr1)) { // if the coordinate was already hit by the second player - update toErase and go again
		toErase.push_back(cr1);
		wasHit.remove(cr1);
		isVertical();
	}
	Coordinate cr2 = { toErase[toErase.size() - 1].row + 1, toErase[0].col, toErase[0].depth };
	if (wasHit.contains(cr2)) { // if the coordinate was already hit by the second player - update toErase and go again
		toErase.push_back(cr2);
		wasHit.remove(cr2);
		isVertical();
	}
	// if the coordinate is still on the board - add it to the "next move" set
	if (boardCoors.contains(cr1)) {
		moves.push_back(cr1);
	}
	if (boardCoors.contains(cr2)) {
		moves.push_back(cr2);
	}
}

void SmartAlgo::isDepthical() {
	sort(toErase.begin(), toErase.end(), [](TupledCoordinate& cr1, TupledCoordinate& cr2) {return CoordinateVector::coordinated(cr1).depth < CoordinateVector::coordinated(cr2).depth; });
	Coordinate cr1 = { toErase[0].row, toErase[0].col, toErase[0].depth - 1 };
	if (wasHit.contains(cr1)) { // if the coordinate was already hit by the second player - update toErase and go again
		toErase.push_back(cr1);
		wasHit.remove(cr1);
		isDepthical();
	}
	Coordinate cr2 = { toErase[0].row, toErase[0].col, toErase[toErase.size() - 1].depth + 1 };
	if (wasHit.contains(cr2)) { // if the coordinate was already hit by the second player - update toErase and go again
		toErase.push_back(cr2);
		wasHit.remove(cr2);
		isDepthical();
	}
	// if the coordinate is still on the board - add it to the "next move" set
	if (boardCoors.contains(cr1)) {
		moves.push_back(cr1);
	}
	if (boardCoors.contains(cr2)) {
		moves.push_back(cr2);
	}
}


ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new SmartAlgo();
}