﻿// MotherOfAllAlgos.cpp : Defines the exported functions for the DLL application.
 //

#include "MotherOfAllAlgos.h"

	using namespace std;

bool SmartAlgo::init(const std::string& path)
{
	/* Initialize data members */
	strategy = HUNTING;
	int rows = board.rows();
	int cols = board.cols();
	int depth = board.depth();
	char*** mBoard = board.getBoard();
	for (int i = 1; i <= rows; i++) {
		for (int j = 1; j <= cols; j++)
			for (int k = 1; k <= depth; k++)
				boardCoors.insert({ i, j, k });
	}

	/* Eliminate points from set */
	for (int i = 1; i <= rows; i++) {
		for (int j = 1; j <= cols; j++) {
			for (int k = 1; k <= depth; k++) {
				set<Coordinate>::iterator it = boardCoors.find({ i, j, k });
				if ((mBoard[i - 1][j - 1][k - 1] != ' ')) {
					if (it != boardCoors.end())
						boardCoors.erase(it);
					removeCross({ i, j, k });
				}
			}
		}
	}
	return true;
}

Coordinate SmartAlgo::attack()
{
	/* Return a no-move if needed (should never reach this) */
	if (boardCoors.empty())
		return{ -1, -1, -1 };

	Coordinate cr = { 0, 0, 0 };
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
		return attack();

	return cr;
}

void SmartAlgo::removeCross(const Coordinate &c)
{
	vector<Coordinate> coors = { { c.row - 1, c.col, c.depth },{ c.row + 1, c.col, c.depth },{ c.row, c.col - 1, c.depth },{ c.row, c.col + 1, c.depth },{ c.row, c.col, c.depth - 1 },{ c.row, c.col, c.depth + 1 } };
	for (Coordinate& cr : coors) {
		set<Coordinate>::iterator it = boardCoors.find(cr);
		if (it != boardCoors.end()) {
			boardCoors.erase(it);
		}
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

vector<Coordinate> SmartAlgo::simulateShipFromCross(Coordinate c, bool explored_only)
{
	vector<Coordinate> simulated_ship = { c };

	/* Find hits in cross */
	int x[6] = { -1, +1, 0, 0, 0, 0 };
	int y[6] = { 0, 0, -1, +1, 0, 0 };
	int z[6] = { 0, 0, 0, 0, -1, +1 };
	for (size_t t = 0; t < 6; t++)
	{
		Coordinate tmp_cr = { c.row + x[t], c.col + y[t], c.depth + z[t] };
		if ((!explored_only && wasHit.find(tmp_cr) != wasHit.end()) || find(toErase.begin(), toErase.end(), tmp_cr) != toErase.end())
		{
			for (auto cr : simulated_ship)
			{
				/* Invalid ship structure */
				if (cr.row != tmp_cr.row && cr.col != tmp_cr.col)
					return{};
			}

			simulated_ship.push_back(tmp_cr);
		}
	}

	return simulated_ship;
}

void SmartAlgo::simulateShipFromVector(vector<Coordinate>& ship_vec)
{
	while (true)
	{
		bool horizontal = false, vertical = false, depthical = false;
		std::sort(ship_vec.begin(), ship_vec.end(), [&horizontal, &vertical, &depthical](Coordinate c1, Coordinate c2)
		{
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

		Coordinate cand_cr1({ 0, 0, 0 }), cand_cr2({ 0, 0, 0 });
		if (horizontal)
		{
			cand_cr1 = { ship_vec[0].first, ship_vec[0].second - 1 };
			cand_cr2 = { ship_vec[0].first, ship_vec[ship_vec.size() - 1].second + 1 };
		}
		else if (vertical)
		{
			cand_cr1 = { ship_vec[0].first - 1, ship_vec[0].second };
			cand_cr2 = { ship_vec[ship_vec.size() - 1].first + 1, ship_vec[0].second };
		}
		else
		{

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

vector<Coordinate> SmartAlgo::simulateShip(Coordinate c, bool explored_only) {
	vector<Coordinate> simulated_ship = simulateShipFromCross(c, explored_only);
	if (simulated_ship.empty())
		return{};

	simulateShipFromVector(simulated_ship);
	return simulated_ship;
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new SmartAlgo();
}