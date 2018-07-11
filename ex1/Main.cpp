#include <string>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "BattleShip.h"



using namespace std;



int main(int argc, char* argv[]) {
	string path = argc == 2 ? string(argv[1]) : "";
	struct stat dir;
	if (argc == 2 && stat(path.c_str(), &dir)) {
		cout << "Wrong path: " << path << endl;
		return 1;
	}
	
	if (argc == 1) {
		path = " ";
	}
	string arg = "Dir " + path + "/b /a-d > filenames.txt";
	system(arg.c_str());
	BattleShipPlayer *player0 = new BattleShipPlayer(0);
	BattleShipPlayer *player1 = new BattleShipPlayer(1);
	BattleShipNavy* navy = new BattleShipNavy();
	dostuffwithfile(path, player0, player1, navy);
	delete player0;
	delete player1;
	delete navy;
	return 0;
}