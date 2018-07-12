#include "BattleshipCompetitionManager.h"
#include <system_error>
void BattleshipCompetitionManager::CompetitionStatistics::sort()
{
	std::sort(mEntries.begin(), mEntries.end(), [](StatEntry & se1, StatEntry & se2) {
		return se1.rate > se2.rate;
	});
}

size_t BattleshipCompetitionManager::CompetitionStatistics::teamWidth()
{
	size_t max = 0;
	for (StatEntry& se : mEntries)
	{
		max = max(max, se.team.length());
	}

	return max + 1;
}

void BattleshipCompetitionManager::CompetitionStatistics::printHeader()
{
	cout << "#" << setw(teamWidth()) << "Team Name" << setw(8) << "Wins" << setw(10)
		<< "Losses" << setw(8) << "%" << setw(8) << "Pts For" << setw(10)
		<< "Pts Against" << endl;

}

void BattleshipCompetitionManager::CompetitionStatistics::registerTeam(size_t pIdx, const string & pTeam)
{
	mEntries.push_back(move(StatEntry(pIdx, pTeam)));
}

void BattleshipCompetitionManager::CompetitionStatistics::addResult(const MatchResult & pRes)
{
	mTableMtx.lock();
	for (size_t i = 0; i < mEntries.size(); i++)
	{
		if (mEntries[i].teamIdx == pRes.homeIdx)
			mEntries[i].addResult(pRes.winner == 0, pRes.homePts, pRes.awayPts);
		if (mEntries[i].teamIdx == pRes.awayIdx)
			mEntries[i].addResult(pRes.winner != 0, pRes.awayPts, pRes.homePts);
	}
	mTableMtx.unlock();
}

void BattleshipCompetitionManager::CompetitionStatistics::print()
{
	unique_lock<mutex> lock(mTableMtx);
	printHeader();
	sort();
	for (size_t i = 0; i < mEntries.size(); i++)
	{
		cout << i + 1 << "." << setw(teamWidth()) << mEntries[i].team << setw(8)
			<< mEntries[i].wins << setw(10) << mEntries[i].losses << setw(8)
			<< mEntries[i].rate << setw(8) << mEntries[i].ptsFor << setw(10) << mEntries[i].ptsAgainst << endl;
	}
}

//from :http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c
bool BattleshipCompetitionManager::SetupManager::hasEnding(string const &fullString, string const &ending)
{
	if (fullString.length() >= ending.length())
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));

	return false;
}

void BattleshipCompetitionManager::SetupManager::parseDirLine(const string& line)
{
	string prefix_path;
	if (mPath == "")
		prefix_path = "";
	else
		prefix_path = mPath + "/";
	//= mPath.empty() ? "" : mPath + "/";

	if (hasEnding(line, ".sboard")) {
		mBoardsString.push_back(prefix_path+line);
	}
	if (hasEnding(line, ".dll")) {
		mDLLs.push_back(prefix_path + line);
	}

	if (hasEnding(line, ".config") && configSearching) {
		confilgFile = prefix_path + line;
		configSearching = false;
	}

}

bool BattleshipCompetitionManager::SetupManager::initFiles()
{
	struct stat dir;
	string numOfThreads, tmp;
	char c;
	if (!mPath.empty() && stat(mPath.c_str(), &dir)) {
		cout << "Wrong path: " << mPath << endl;;
		return false;
	}

	char buffer[4096];
	string data_str;
	string arg = "2>NUL dir /b /a-d \"" + mPath + "\"";
	FILE* fp = _popen(arg.c_str(), "r");

	while (fgets(buffer, 4095, fp))
		data_str += string(buffer);
	string line;
	istringstream data_str2(data_str);
	while (getline(data_str2, line))
		parseDirLine(line);
	if (confilgFile != "") { // Accepting syntax "thread : num"
		ifstream infile;
		infile.open(confilgFile);
		getline(infile, numOfThreads); 
		std::stringstream ss(numOfThreads);
		ss >> tmp >> c >> value;
	}
	else {
		value = 4;
	}
	if (value <1){
		cout << "Error: wrong command line args. " << endl;
		return false;
	}
	_pclose(fp);

	return error();
}

bool BattleshipCompetitionManager::SetupManager::error() const
{
	bool valid = true;

	if (mBoardsString.empty())
	{
		cout << "No board files (*.sboard)" << endl;
		valid = false;
	}

	if (mDLLs.size() < 2)
	{
		cout << "Missing algorithm(dll) files" << endl; ;
		valid = false;
	}

	return valid;
}

bool BattleshipCompetitionManager::initFromPath(const string & pPath)
{
	/* Check if all necessary files exist */
	setup_manager.mPath = pPath;
	bool files_exist = setup_manager.initFiles();
	if (files_exist) {
		mLogger->initLog(pPath);
		validateBoards();
		validateDLLs();
	}
	return files_exist;
}

void BattleshipCompetitionManager::createArray(ifstream& board_file)
{
	int cols, rows, depth;
	string line;
	getline(board_file, line);
	std::istringstream iss(line);
	std::string token;
	try {
		getline(iss, token, 'x');
		cols = stoi(token);
		getline(iss, token, 'x');
		rows = stoi(token);
		getline(iss, token, 'x');
		depth = stoi(token);
	}
	catch (const invalid_argument& ia) {
		(void)ia;
		return;
	}
	shared_ptr<BattleshipBoard> bsBoard(new BattleshipBoard(rows, cols, depth));
	for (int m = 0; m < depth; m++) { //the depth
		if (m == 0)
			getline(board_file, line);
		int i = 0;
		getline(board_file, line);
		while ((line != "") && (i != rows)) {
			bsBoard->parseBoardLine(m, i, line);
			i++;
			getline(board_file, line);
		}
		if (line == "") { //fill the rest with spaces
			for (int j = i; j < rows; j++)
				bsBoard->parseBoardLine(m, j, line);
		}
	}
	BuildNavyLog(bsBoard);
	return;
}

void  BattleshipCompetitionManager::BuildNavyLog(shared_ptr<BattleshipBoard> bsBoard) {
	tuple<string, string> msgs = bsBoard->buildNavy();
	logMsgTypes = std::get<0>(msgs);
	logMsgAmount = std::get<1>(msgs);
	if (logMsgTypes != "" || logMsgAmount != "") {
		{
			std::lock_guard<std::mutex> lk(logMutex);
			logReady = true;
		}
		logCv.notify_one();
		// wait for the log thread
		{
			std::unique_lock<std::mutex> lk(logMutex);
			//logCv.wait(lk, [](){return true; });
			logCv.wait(lk, [=]() {return logged; });
			logged = false;
		}
	}
	if (bsBoard->getNavy().isValid()) {
		mBoards.push_back(bsBoard);
	}
}

void BattleshipCompetitionManager::CompetitionStatistics::StatEntry::addResult(bool victory, int points_for, int points_against)
{
	statMtx->lock();
	ptsFor += points_for;
	ptsAgainst += points_against;
	if (victory)
		wins++;
	else
		losses++;
	rate = 100.0 * wins / (wins + losses);
	statMtx->unlock();
}
void BattleshipCompetitionManager::FixturesManager::genFixtures()
{
	int nPlayer = numOfPlayers + numOfPlayers % 2;
	int rounds = nPlayer - 1;
	for (int boardIdx = 0; boardIdx < numOfBoards; boardIdx++)
	{
		vector<int> fixtures, rotation;
		for (int i = 0; i < nPlayer; i++) {
			rotation.push_back(i);
		}
		for (int r = 0; r < rounds; r++)
		{
			fixtures.insert(fixtures.end(), rotation.begin(), rotation.end());
			vector<int>	tmp_rotation;
			tmp_rotation.insert(tmp_rotation.end(), rotation.begin() + 1, rotation.end() - 1);
			rotation = { rotation[0], rotation[rotation.size() - 1] };
			rotation.insert(rotation.end(), tmp_rotation.begin(), tmp_rotation.end());
		}

		vector<int> oppositeFixtures = fixtures;
		reverse(begin(oppositeFixtures), end(oppositeFixtures));
		fixtures.insert(fixtures.end(), oppositeFixtures.begin(), oppositeFixtures.end());
		for (size_t i = 0; i < fixtures.size(); i++)
		{
			int homeIdx = fixtures[i], awayIdx = fixtures[++i];
			if (numOfPlayers % 2 && (homeIdx == numOfPlayers || awayIdx == numOfPlayers))
				continue;
			size_t fixtures_inarr = mFixtures.size();
			pair<int, int> players = { homeIdx, awayIdx };
			tuple<int, int, pair<int, int>> match = make_tuple(fixtures_inarr / (numOfPlayers / 2), boardIdx, players);
			mFixtures.push_back(match);
		}
	}
}
BattleshipCompetitionManager::FixturesManager::FixtureInfo BattleshipCompetitionManager::FixturesManager::nextFixture() const
{
	static size_t i = 0;
	if (i < mFixtures.size())
		return mFixtures[i++];

	return make_tuple(-1, -1, make_pair(-1, -1));
}
void BattleshipCompetitionManager::MatchPool::start(size_t threads)
{
	for (size_t i = 0; i < threads; i++)
	{
		m_Threads.emplace_back([this]() -> void {
			while (true)
			{
				FixturesManager::FixtureInfo info;
				{
					unique_lock<mutex> lock(m_Mtx);
					m_TaskCv.wait(lock, [this]() -> bool {
						return !m_Tasks.empty() || _stop;
					});

					if (_stop && m_Tasks.empty())
						return;

					info = m_Tasks.front();
					m_Tasks.pop();
				}
				int matchnum = m_Played++;
				CompetitionStatistics::MatchResult result = m_Matcher(info);
				{
					unique_lock<mutex> lock(m_ResultsMtx);
					m_Results[get<0>(info)].push_back(result);
					if (m_Results[round].size() == matchesPerRound)
					{
						m_PrintCv.notify_one();
					}
				}
				m_Counter--;
			}
		});
	}
}
void BattleshipCompetitionManager::MatchPool::printer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m_ResultsMtx);
			m_PrintCv.wait(lock, [this]() -> bool {
				return m_Results[round].size() == matchesPerRound;
			});

			for (CompetitionStatistics::MatchResult& res : m_Results[round])
				m_Resulter(res);
			m_Printer();

			round++;
			if (round == m_Results.size())
				break;
		}
	}
}
void BattleshipCompetitionManager::MatchPool::push(FixturesManager::FixtureInfo info)
{
	{
		unique_lock<mutex> lock(m_Mtx);
		m_Tasks.push(info);
	}
	m_Counter++;
	m_TaskCv.notify_one();
}

void BattleshipCompetitionManager::MatchPool::wait()
{
	while (m_Counter != 0);
	this_thread::sleep_for(chrono::milliseconds(1));
	_stop = true;
	m_TaskCv.notify_all();
	for (thread & t : m_Threads)
		t.join();
	m_PrintThread.join();
}
void BattleshipCompetitionManager::createDLL(const string & dll)
{
	HINSTANCE Dll = LoadLibraryA(dll.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (Dll)
	{
		GetAlgorithmType getAlgoFunc = (GetAlgorithmType)GetProcAddress(Dll, "GetAlgorithm");
		if (getAlgoFunc)
		{
			mAlgos.push_back(getAlgoFunc);
		}
	}
	size_t indx = dll.find_last_of('/');

	mStats.registerTeam(mAlgos.size() - 1, dll.substr(indx + 1));
}

void BattleshipCompetitionManager::validateBoards()
{
	for (const string & sb : setup_manager.mBoardsString) {
		createArray(ifstream(sb));
	}
}

void BattleshipCompetitionManager::validateDLLs()
{
	for (const string & dll : setup_manager.mDLLs)
		createDLL(dll);
}

void BattleshipCompetitionManager::printPreGameInfo()
{
	cout << "Number of legal players: " << mAlgos.size() << endl;
	cout << "Number of legal boards: " << mBoards.size() << endl;
}

void BattleshipCompetitionManager::initMatches()
{
	fixtures_manager.numOfBoards = static_cast<int>( mBoards.size());
	fixtures_manager.numOfPlayers = static_cast<int>(mAlgos.size());
	fixtures_manager.genFixtures();
}

BattleshipCompetitionManager::CompetitionStatistics::MatchResult BattleshipCompetitionManager::runSingleMatch(FixturesManager::FixtureInfo fi)
{
	shared_ptr<BattleshipBoard> board = mBoards[get<1>(fi)];
	shared_ptr<IBattleshipGameAlgo> algo1(mAlgos[get<2>(fi).first]());
	shared_ptr<IBattleshipGameAlgo> algo2(mAlgos[get<2>(fi).second]());
	unique_ptr<BattleshipManager> sub_manager(new  BattleshipManager(*board, algo1, algo2));

	tuple<int, int, int> res = sub_manager->Manage();
	//sub_manager.release();
	CompetitionStatistics::MatchResult matchRes;
	matchRes.homeIdx = get<2>(fi).first;
	matchRes.awayIdx = get<2>(fi).second;
	matchRes.winner = get<0>(res);
	matchRes.homePts = get<1>(res);
	matchRes.awayPts = get<2>(res);

	return matchRes;
}

void BattleshipCompetitionManager::run()
{
	if (mThreads == 0) {
		mThreads = setup_manager.value;
		if (mThreads <= 0)
			return;
	}
	size_t rounds = 2 * (mAlgos.size() - (mAlgos.size() % 2 == 0)) * mBoards.size();
	printPreGameInfo();
	initMatches();
	if (fixtures_manager.mFixtures.size() == 0)
		return;
	unique_ptr<MatchPool> pool = make_unique<MatchPool>(mThreads, rounds, mAlgos.size() / 2);
	pool->setPrinter([this]() {mStats.print(); });
	pool->setTableManager([this](CompetitionStatistics::MatchResult res) {this->mStats.addResult(res); });
	pool->setMatcher([this](FixturesManager::FixtureInfo info) {return this->runSingleMatch(info); });
	FixturesManager::FixtureInfo fixture;
	FixturesManager::FixtureInfo end = make_tuple(-1, -1, make_pair(-1, -1));
	while ((fixture = fixtures_manager.nextFixture()) != end)
	{

		pool->push(fixture);
	}
	pool->wait();
	//pool.release();
}

void BattleshipCompetitionManager::log_thread()
{
	while (living) {
		std::unique_lock<std::mutex> lk(logMutex);
		logCv.wait(lk, [=]() {return logReady; });
		if (living == false)
			return;
		// after the wait, we own the lock and we can write data to the logger.
		if (logMsgTypes != "") {
			*LOGGER << logMsgTypes;
		}
		if (logMsgAmount != "") {
			*LOGGER << logMsgAmount;
		}
		logMsgTypes = "";
		logMsgAmount = "";
		// Manual unlocking is done before notifying, to avoid waking up the waiting thread only to block again
		lk.unlock();
		logReady = false;
		logged = true;
		logCv.notify_one();
	}
}