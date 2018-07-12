#include "Types.h"
#include "BattleshipManager.h"

using namespace std;

#define RETURN_ERR(msg) cout << msg << endl; RETURN(1);
#define RETURN(n) delete manager; return n;

int main(int argc, char* argv[])
{
		bool path_given = (argc > 1 && strncmp(argv[1], "-", 1) != 0);
		BattleshipManager * manager;
		if (path_given)
			manager = new BattleshipManager(argv[1]);
		else
			manager = new BattleshipManager("");
		int i = 1;
		if (path_given) i++;
		for (i; i < argc; i++)
		{
			if (strcmp(argv[i], "-quiet") == 0) {
				manager->utils()->setQuiet();
			}

			else if (strcmp(argv[i], "-delay") == 0)
			{
				if (i == argc - 1) { RETURN_ERR("Error: wrong command line args. ") }

				manager->utils()->setDelay(atoi(argv[++i]));
			}

			else { RETURN_ERR("Error: wrong command line args. ") }
		}
		if (!manager->setup()) { RETURN(1) }
		//manager->play();
		RETURN(0)

}