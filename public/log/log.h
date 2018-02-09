#ifndef __PUBLIC_LOG_LOG_H__
#define __PUBLIC_LOG_LOG_H__
#include <iostream>
#include <stdarg.h>
#include <pthread.h>
#include <string>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "../util/util.h"

#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

class CLog
{
public:

	static CLog& getInstance();

	void init_log();

	void log(int level, const char* color, const char* file, int line, const char* fmt, ...);

	static void* check_log_file(void*);

	bool isRun(){return m_bRun;}

	void checkLogSize();

	void createLogFile();

private:
	CLog();

	~CLog();
private:
	int m_nLevel;
	int m_nLogSize;
	std::string m_strLogPath;
	pthread_t m_nChecklogId;
	bool m_bRun;
	int m_nFdNo;
	std::string m_strLogName;
};

#define CLogMgr CLog::getInstance()

#define INFO(fmt, args...) \
	CLogMgr.log(1, "", filename(__FILE__), __LINE__, fmt, ##args);

#endif