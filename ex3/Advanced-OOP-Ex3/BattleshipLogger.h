#ifndef BATTLESHIP_LOGGER_H
#define BATTLESHIP_LOGGER_H

#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <memory>

using namespace std;

#define LOGGER BattleshipLogger::getLogger()

/*
*   Singleton Logger Class.
*/
class BattleshipLogger
{
	ofstream							mLogfile;
	static shared_ptr<BattleshipLogger>	mLogger;


	/**
	*   copy constructor for the Logger class.
	*/
	BattleshipLogger(const BattleshipLogger &){}

	/**
	*   assignment operator for the Logger class.
	*/
	BattleshipLogger& operator=(const BattleshipLogger &) { return *this; };


public:
	BattleshipLogger(){}
	~BattleshipLogger(){}

	void initLog(const string & pDir);
	/*
	*   Logs a message
	*   @param sMessage message to be logged.
	*/
	void log(const string & sMessage);
	/**
	*   << overloaded function to Logs a message
	*   @param sMessage message to be logged.
	*/
	BattleshipLogger& operator<<(const string & sMessage);
	/**
	*   Funtion to create the instance of logger class.
	*   @return singleton object of Clogger class..
	*/
	static shared_ptr<BattleshipLogger> getLogger();
};

#endif