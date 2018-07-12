#ifndef BATTLESHIP_COMPETITION_MANAGER_H
#define BATTLESHIP_COMPETITION_MANAGER_H

#include "BattleshipLogger.h"
#include "BattleshipManager.h"
#include "BattleShipLayouts.h"
#include "Types.h"

using namespace std;

typedef IBattleshipGameAlgo * (*GetAlgorithmType)();

class BattleshipCompetitionManager
{
	/* Competition utilities */
	int										mThreads;
	vector<GetAlgorithmType>				mAlgos;
	vector<shared_ptr<BattleshipBoard>>		mBoards;

	/* Logger utilities */
	shared_ptr<BattleshipLogger>			mLogger = LOGGER;
	string									logMsgTypes;
	string									logMsgAmount;
	mutex									logMutex;
	condition_variable						logCv;
	bool									logReady = false;
	bool									living = true;
	bool									logged = false;
	thread									logThread;

public:

	/* Manages the statistics throughout the entire competition */
	class CompetitionStatistics
	{
	public:
		struct MatchResult
		{
			size_t	homeIdx, awayIdx;
			int		winner;
			int		homePts, awayPts;
		};

	private:
		struct StatEntry
		{
			size_t		teamIdx;
			string		team;
			int			wins, losses;
			double		rate;
			int			ptsFor, ptsAgainst;
			shared_ptr<mutex>statMtx;

			StatEntry(size_t pIdx, const string & pTeam)
				:teamIdx(pIdx), team(pTeam), wins(0), losses(0), rate(0.0), ptsFor(0), ptsAgainst(0), statMtx(new mutex()) {}

			
			void addResult(bool victory, int points_for, int points_against);
		};

		vector<StatEntry>	mEntries;
		mutex				mTableMtx;

		void sort();
		void printHeader();
		size_t teamWidth();

	public:
		CompetitionStatistics() {}
		virtual ~CompetitionStatistics() {}

		void registerTeam(size_t pIdx, const string & pTeam);
		void addResult(const MatchResult & pRes);
		void print();
	}	mStats;

	/* Handles creation of matches order */
	struct FixturesManager
	{
		/* Tuple consists of <board num, <player1, player2>> */
		typedef tuple<int, int, pair<int, int>> FixtureInfo;

		int					numOfPlayers, numOfBoards;
		vector<FixtureInfo>	mFixtures;

		void genFixtures();
		FixtureInfo nextFixture() const;
	} fixtures_manager;

	/* Sets up all boards and algorithms */
	struct SetupManager
	{
		string			mPath, confilgFile = "";
		vector<string>	mBoardsString, mDLLs;
		bool			configSearching = true;
		int				value;


		bool initFiles();
		bool error() const;
		void parseDirLine(const string& line);
		static bool hasEnding(string const &fullString, string const &ending);
	} setup_manager;

	/* Represents the configuration of a single match */
	struct MatchInfo
	{
		BattleshipBoard		board;
		GetAlgorithmType	p1, p2;

		MatchInfo(BattleshipBoard & pBoard, GetAlgorithmType pAlgo1, GetAlgorithmType pAlgo2)
			:board(pBoard), p1(pAlgo1), p2(pAlgo2) {}
	};

	/* ThreadPool for matches */
	class MatchPool
	{
		/* Typedefs for relevant functions */
		typedef function<CompetitionStatistics::MatchResult(FixturesManager::FixtureInfo)> MatchFunc;
		typedef function<void(CompetitionStatistics::MatchResult)> ResultFunc;
		typedef function<void(void)> PrintFunc;

		vector<thread>											m_Threads;
		vector<vector<CompetitionStatistics::MatchResult>>		m_Results;
		thread													m_PrintThread;
		queue<FixturesManager::FixtureInfo>						m_Tasks;
		atomic<int>												m_Counter, m_Played;
		mutex													m_Mtx, m_ResultsMtx;
		condition_variable										m_TaskCv, m_PrintCv;
		atomic<bool>											_stop;
		PrintFunc												m_Printer;
		MatchFunc												m_Matcher;
		ResultFunc												m_Resulter;
		int														matchesPerRound, round;

		void start(size_t threads);

	public:
		void printer();

	public:
		MatchPool(size_t threads, int rounds, int mpr)
			:matchesPerRound(mpr), round(0), m_Mtx(), m_Results(rounds), m_ResultsMtx(),
			m_TaskCv(), m_PrintCv(), _stop(false), m_Counter(0), m_Tasks(), m_Threads()
		{
			start(threads);
			m_PrintThread = thread([=]() {this->printer(); });
		}

		~MatchPool() {}

		void setPrinter(PrintFunc f) { m_Printer = f; }
		void setMatcher(MatchFunc f) { m_Matcher = f; }
		void setTableManager(ResultFunc f) { m_Resulter = f; }
		void push(FixturesManager::FixtureInfo info);
		void wait();
	};

public:
	/*
	* C'tor for the competition manager.
	* @param pThreads - Number of threads to run simultaneously.
	*/
	BattleshipCompetitionManager(int pThreads = 0) :mThreads(pThreads), logThread([=] {log_thread(); }) {}
	/*
	* D'tor
	*/
	~BattleshipCompetitionManager() {
		living = false;
		logCv.notify_all();
		logReady = true;
		logThread.join();
	}

	/* Avoid using copy c'tor and copy assignments */
	BattleshipCompetitionManager(const BattleshipCompetitionManager &) = delete;
	BattleshipCompetitionManager& operator=(const BattleshipCompetitionManager &) = delete;

public:
	/*
	* Initialize DLL files and board files.
	* @param pPath - Directory to read files from.
	*
	* @return TRUE iff directory contains relevant and legal files.
	*/
	bool initFromPath(const string & pPath);

	/*
	* Run competition using the ThreadPool
	*/
	void run();

	/*
	* Runs a single match.
	* @param fi - Match configuration
	* @return the struct with the match results
	*/
	CompetitionStatistics::MatchResult runSingleMatch(FixturesManager::FixtureInfo fi);

private:
	void  BattleshipCompetitionManager::BuildNavyLog(shared_ptr<BattleshipBoard> bsBoard);
	void createArray(ifstream & board_file);
	void createDLL(const string & dll);
	void validateBoards();
	void validateDLLs();

	/*
	* Print boards and players info.
	*/
	void printPreGameInfo();
	void initMatches();
	void log_thread();
};

#endif