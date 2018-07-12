#include "Types.h"
#include "BattleshipCompetitionManager.h"

using namespace std;

#define RETURN_ERR(msg) cout << msg << endl; RETURN(1);
#define RETURN(n) delete manager; return n;

int main(int argc, char* argv[])
{
	BattleshipCompetitionManager* manager;
	bool path_given = (argc > 1 && string(argv[1]) != "-threads");
	int i = 1 + path_given;
	if (argc > i)
	{
		if (string(argv[i]) != "-threads")
		{
			cout << "Error: wrong command line args. " << endl;
			return -1;
		}

		if (argc != i + 2)
		{
			cout << "Error: wrong command line args. " << endl;
			return -1;
		}

		try
		{
			int numThreads = stoi(string(argv[i + 1]));
			if (numThreads < 1)
			{
				cout << "Error: wrong command line args. " << endl;
				return -1;
			}

			manager = new BattleshipCompetitionManager(numThreads);
		}
		catch (const invalid_argument & ia)
		{
			(void)ia;
			return -1;
		}
	}
	else
		manager = new BattleshipCompetitionManager();
	
	bool valid_path = false;

	if (path_given) {
		valid_path = manager->initFromPath(argv[1]);
	}

	else{
		const unsigned long maxDir = 260;
		char currentDir[maxDir];
		GetCurrentDirectoryA(maxDir, currentDir);
		valid_path = manager->initFromPath(string(currentDir));
	}
	if (valid_path && manager){
		manager->run();
	}
	delete manager;
	return 0;
}