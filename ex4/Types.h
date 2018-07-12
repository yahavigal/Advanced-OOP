#pragma once
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <set>

using namespace std;

template<typename T, size_t DIM>
class Coordinate
{
	constexpr static size_t DIMENSION = DIM;

	T				value;
	vector<int>		coord;

public:
	Coordinate()
		:coord(DIMENSION) {}
	Coordinate(vector<int> & pCoord, T & pValue)
		:value(pValue)
	{
		size_t i = 0;
		for (; i < min(DIMENSION, pCoord.size()); i++)
			coord.push_back(pCoord[i]);
		for (; i < DIMENSION; i++)
			coord.push_back(0);
	}

	Coordinate(const Coordinate<T, DIM> & c) : coord(DIMENSION)
	{
		*this = c;
	}
	Coordinate(const Coordinate<T, DIM> && c)
	{
		std::swap(coord, c.coord);
		std::swap(value, c.value);
	}

	T getValue() const { return value; }
	void setValue(T pVal) { value = pVal; }

	bool operator<(const Coordinate<T, DIM> & c) const
	{
		if (*this == c)
			return false;
		for (size_t i = 0; i < DIMENSION; i++)
		{
			if (coord[i] < c.coord[i]) {
				return true;
			}
			if (coord[i] > c.coord[i]) {
				return false;
			}
		}
		return true;
	}

	bool adjacent(const Coordinate<T, DIM> & c)
	{
		int diff = 0;
		for (size_t i = 0; i < DIMENSION; i++)
			diff += abs(coord[i] - c.coord[i]);
		return diff == 1;
	}

	bool operator==(const Coordinate<T, DIM> & c) const
	{
		for (size_t i = 0; i < DIMENSION; i++) {
			if (coord[i] != c.coord[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(const Coordinate<T, DIM> & c)
	{
		return !(*(this) == c);
	}

	Coordinate<T, DIM>& operator=(const Coordinate<T, DIM> & c)
	{
		if (*(this) != c)
		{
			coord = c.coord;
		}
		value = c.value;

		return *this;
	}

	Coordinate<T, DIM>& operator+(const Coordinate<T, DIM> & c)
	{
		for (size_t i = 0; i < DIMENSION; i++)
			coors[i] += c / coors[i];

		return *this;
	}

	int& operator[](size_t i)
	{
		return coord[i];
	}

	std::vector<int>::const_iterator begin() const
	{
		return coord.cbegin();
	}

	std::vector<int>::const_iterator end() const
	{
		return coord.cend();
	}
};

template<typename T, size_t DIM>
class Battleship {
	T type;
	set<Coordinate<T, DIM>> coors;
public:
	Battleship(const Coordinate<T, DIM> & c)
	{
		type = c.getValue();
		coors.insert(c);
	}
	Battleship(set<Coordinate<T, DIM>> sit)
	{
		coors = sit;
		type = sit.begin()->getValue();
	}

	~Battleship()
	{
	}
	bool findCoor(Coordinate<T, DIM> co) {
		return (coors.find(co) != coors.end())

	}
	bool isNeighbor(const Coordinate<T, DIM>& c)
	{
		if (c.getValue() != type)
			return false;
		for (auto &coor : coors)
		{
			for (int i = 0; i < DIM; i++)
			{
				Coordinate<T, DIM> tmp = coor;
				if (tmp.adjacent(c))
					return true;
			}
		}
		return false;
	}


	bool inCross(const Battleship<T, DIM> ship)
	{
		for (auto &coor : ship.coors)
		{
			for (auto &c : coors)
			{
				for (int i = 0; i < DIM; i++)
				{
					Coordinate<T, DIM> tmp = c;
					if (tmp.adjacent(coor))
						return true;
				}
			}
		}
		return false;
	}

	void add(const Coordinate<T, DIM>& c)
	{
		coors.insert(c);
	}

	T getType() const
	{
		return type;
	}

	typename std::set<Coordinate<T, DIM>>::iterator begin() const
	{
		return coors.begin();
	}

	typename std::set<Coordinate<T, DIM>>::iterator end() const
	{
		return coors.end();
	}

	void operator+=(const Battleship<T, DIM>& ship)
	{
		auto beg = ship.coors.begin();
		auto en = ship.coors.end();
		coors.insert(beg, en);
	}
};