#include "BattleshipLogger.h"
#include "LoggerUtils.h"

using namespace std;

shared_ptr<BattleshipLogger> BattleshipLogger::mLogger = nullptr;

void BattleshipLogger::initLog(const string & pDir)
{
	string filename = pDir + (pDir.empty() ? "" : "\\") + "game.log";
	mLogfile.open(filename.c_str(), ios::out | ios::trunc);
	log("Let the games begin! May the odds be ever in your favor...");
}

shared_ptr<BattleshipLogger> BattleshipLogger::getLogger()
{
	if (!mLogger)
		mLogger = make_shared<BattleshipLogger>();

	return mLogger;
}

void BattleshipLogger::log(const string & sMessage)
{
	mLogfile << LoggerUtils::CurrentDateTime() << ":\t";
	mLogfile << sMessage << "\n";
}

BattleshipLogger& BattleshipLogger::operator<<(const string & sMessage)
{
	mLogfile << "\n" << LoggerUtils::CurrentDateTime() << ":\t";
	mLogfile << sMessage << "\n";
	return *this;
}