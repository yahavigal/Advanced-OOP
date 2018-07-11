#include "Types.h"

using namespace std;
typedef tuple<int, int, int> TupledCoordinate;

class CoordinateVector
{
	vector<TupledCoordinate>	_vector;

public:
	CoordinateVector(){}
	~CoordinateVector(){ _vector.clear(); }

	static TupledCoordinate tupled(const Coordinate & c)
	{
		return make_tuple(c.row, c.col, c.depth);
	}

	static Coordinate coordinated(TupledCoordinate c)
	{
		return Coordinate(get<0>(c), get<1>(c), get<2>(c));
	}

	vector<TupledCoordinate>::iterator find(const Coordinate & c)
	{
		return std::find(_vector.begin(), _vector.end(), tupled(c));
	}

	vector<TupledCoordinate>::iterator begin()
	{
		return _vector.begin();
	}

	vector<TupledCoordinate>::iterator end()
	{
		return _vector.end();
	}

	void push_back(const Coordinate & c)
	{
		_vector.push_back(tupled(c));
	}

	void clear()
	{
		_vector.clear();
	}

	size_t size() const
	{
		return _vector.size();
	}

	bool empty() const
	{
		return _vector.empty();
	}

	bool contains(const Coordinate & c)
	{
		return find(c) != _vector.end();
	}

	void erase(const Coordinate & c)
	{
		if (contains(c))
			_vector.erase(find(c));
	}

	Coordinate& operator[](size_t i)
	{
		return coordinated(_vector[i]);
	}

	Coordinate back()
	{
		return coordinated(_vector.back());
	}

	void pop_back()
	{
		_vector.pop_back();
	}
};

class CoordinateSet
{
	set<TupledCoordinate>		_set;

	static TupledCoordinate tupled(const Coordinate & c)
	{
		return make_tuple(c.row, c.col, c.depth);
	}

	static Coordinate coordinated(TupledCoordinate c)
	{
		return Coordinate(get<0>(c), get<1>(c), get<2>(c));
	}


public:
	CoordinateSet(){}
	~CoordinateSet(){ _set.clear(); }

	void fill(int rows, int cols, int depth)
	{
		for (int i = 1; i <= rows; i++)
		{
			for (int j = 1; j <= cols; j++)
			{
				for (int k = 1; k <= depth; k++)
					_set.insert(tupled({ i, j, k }));
			}
		}
	}

	set<TupledCoordinate>::iterator find(const Coordinate & c) const
	{
		return _set.find(tupled(c));
	}

	Coordinate first()
	{
		if (!empty())
			return coordinated(*(_set.begin()));
		return{ 0, 0, 0 };
	}

	bool contains(const Coordinate & c) const
	{
		return find(c) != _set.end();
	}

	void clear()
	{
		return _set.clear();
	}

	void remove(const Coordinate & c)
	{
		if (contains(c))
			_set.erase(find(c));
	}

	bool insert(const Coordinate & c)
	{
		return _set.insert(tupled(c)).second;
	}

	Coordinate random() const
	{
		random_device rd;
		default_random_engine dre(rd());
		set<tuple<int, int, int>>::iterator it = _set.begin();
		advance(it, dre() % _set.size());
		return coordinated(*it);
	}

	size_t size() const
	{
		return _set.size();
	}

	bool empty() const
	{
		return _set.empty();
	}
};