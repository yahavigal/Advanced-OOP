#include "Ex1Algo.h"

bool Ex1Algo::hasEnding(string const &fullString, string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

bool Ex1Algo::init(const std::string& path)
{
	char buffer[4096];
	string newPath;
	bool AttackFound = false;
	string Attack_file;
	std::string data_str;;
	if (path == "")
		newPath = ".\\";

	string arg = "2>NUL dir /b /a-d \"" + path + "\"";
	FILE* fp = _popen(arg.c_str(), "r");
	while (fgets(buffer, 4095, fp))
		data_str += string(buffer);

	_pclose(fp);
	std::string line;
	std::istringstream data_str2(data_str);
	while (std::getline(data_str2, line)) {
		
		if (!AttackFound&&  hasEnding(line, ".attack"))
		{
			AttackFound = true;
			Attack_file = line;
			if (Player == 0)
				break;
			continue;
		}
		if (AttackFound&&  Player==1 && hasEnding(line, ".attack"))
		{
			Attack_file = line;
			break;
		}
	}
	if (AttackFound == false) {
		return false;
	}
	if (path == "")
		setMoves(ifstream(newPath + Attack_file));
	else
		setMoves(ifstream(path+"/"+Attack_file));
	return true;
}

void Ex1Algo::parseLine(const string& line)
{
	pair<int, int> p;
	bool commaFound = false;
	bool numberFound = false;
	string n1(""), n2("");
	for (size_t t = 0; t < line.size(); t++)
	{
		if (isspace(line[t])) continue;
		if (line[t] == ',') {
			commaFound = true;
			numberFound = false;
			try {
				p.first = stoi(n1.c_str());
			}
			catch (const std::invalid_argument& ia)
			{
				(void)ia;
				return;
			}

			continue;
		}
		if (!commaFound) {
			if (!numberFound) {
				if ((line[t] < '0') || (line[t] > '9')) return;
				n1 += line[t];
				numberFound = true;
			}
			else {
				if (line[t] != '0') return;
				n1 += line[t];
			}
		}
		else {
			if (!numberFound) {
				if ((line[t] < '0') || (line[t] > '9')) return;
				n2 += line[t];
				numberFound = true;
			}
			else {
				if (line[t] != '0') return;
				n2 += line[t];
				break;
			}
		}
	}
	try {
		p.second = stoi(n2.c_str());
	}
	catch (const std::invalid_argument& ia) {
		(void)ia;
		return;
	}
	
	moves.push(p);
}

void Ex1Algo::setMoves(ifstream& file) {
	
	string line;
	int i = 0;
	while (getline(file, line))
		parseLine(line);
}



std::pair<int, int> Ex1Algo::attack()
{
	if (moves.empty())
		return{ -1,-1 };

	Point p = moves.front();
	
	moves.pop();
	return{ p.first,p.second };
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new Ex1Algo();
}