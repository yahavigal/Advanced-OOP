#include "Naive.h"

using namespace std;

bool NaiveAlgo::init(const std::string& path) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (matrix[i][j] != ' ') continue;
			if (i != 0 && matrix[i - 1][j] != ' ') continue;
			if (j != 0 && matrix[i][j - 1] != ' ') continue;
			if (i != rows - 1 && matrix[i + 1][j] != ' ') continue;
			if (j != cols - 1 && matrix[i][j + 1] != ' ') continue;

			moves.push({ i + 1, j + 1 });
		}
	}

	return true;
}

std::pair<int, int> NaiveAlgo::attack() {
	Point p;

	if (!moves.empty()) {
		p = moves.front();
		moves.pop();
	}
	else
		p = { -1, -1 };

	return p;
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new NaiveAlgo();
}