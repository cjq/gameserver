#include "log.h"
#include "../config/config.h"

#include <string>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

CLog::CLog()
{

}

CLog::~CLog()
{
	m_nLevel = 0;
	m_nLogSize = 0;
	m_bRun = false;
	m_nFdNo = -1;
}

CLog& CLog::getInstance()
{
	static CLog log;
	return log;
}

void CLog::init_log()
{
	std::string strSize;
	ConfigMgr.getConfigInfo("LOG_SIZE", strSize);
	if (strSize.empty())
	{
		printf("CLog::init_log read log_size error\n");
		return;
	}

	m_nLogSize = atoi(strSize.c_str());
	if (m_nLogSize < 1024)
	{
		m_nLogSize = 1024 * 1024;
	}
	printf("log_size : %d\n", m_nLogSize);

	
	ConfigMgr.getConfigInfo("LOG_PATH", m_strLogPath);
	if (m_strLogPath.empty())
	{
		m_strLogPath = "./log";
	}
	printf("log_path : %s\n", m_strLogPath.c_str());
	m_bRun = true;
	createLogFile();
	if (pthread_create(&m_nChecklogId, NULL, check_log_file, this))
	{
		perror("create check_log_file thread error");
	}
}

void* CLog::check_log_file(void* param)
{
	CLog* pLog = (CLog*)param;
	if (NULL != pLog)
	{
		while (pLog->isRun())
		{
			sleep(60);
			pLog->checkLogSize();
		}
	}
	else
	{
		printf("CLog::check_log_file param error\n");
	}
	return 0;
}

void CLog::log(int level, const char* color, const char* file, int line, const char* fmt, ...)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t now_t;
	struct tm* tm_now;
	time(&now_t);
	tm_now = localtime(&now_t);
	printf("%s[%04d-%02d-%02d %02d:%02d:%02d:%03d %s %d] ",
		color,tm_now->tm_year+1900, tm_now->tm_mon+1,tm_now->tm_mday, 
		tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, 
		(int)(tv.tv_usec / 1000), file, line);

	va_list ap;
	va_start(ap,fmt);
	vfprintf(stdout,fmt,ap);
	va_end(ap);

	if(level > 1) printf(NONE"\n");
	else printf("\n");
	fflush(stdout);
}

void CLog::checkLogSize()
{
	if (m_nFdNo < 0)
	{
		return;
	}

	int size = lseek(m_nFdNo, 0, SEEK_END);
	if (size > m_nLogSize)
	{
		createLogFile();
		return;
	}

	if (access(m_strLogName.c_str(), F_OK) == -1)
	{
		createLogFile();
		return;
	}
}

void CLog::createLogFile()
{
	if (m_nFdNo > 0)
	{
		close(m_nFdNo);
	}

	std::string oldLogName = m_strLogName;
	m_strLogName.clear();
	time_t now_t;
	struct tm* tm_now;
	time(&now_t);
	tm_now = localtime(&now_t);

	char newLogName[200];
	sprintf(newLogName, "%s/%04d%02d%02d%02d%02d%02d.log",
		m_strLogPath.c_str(), tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour,
		tm_now->tm_min, tm_now->tm_sec);
	std::string newStrLogName(newLogName);
	m_strLogName.swap(newStrLogName);

	m_nFdNo = open(m_strLogName.c_str(), O_CREAT | O_RDWR, S_IREAD | S_IWRITE |
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (m_nFdNo < 0)
	{
		printf("open file:%s error", m_strLogPath.c_str());
		return;
	}

	dup2(m_nFdNo, 1);
	INFO("new file:%s, fd:%d", m_strLogName.c_str(), m_nFdNo);
	if (oldLogName.size() > 0)
	{
		char cmd[200] = {0,};
		sprintf(cmd, "tar zcvf %s.tar.gz %s; rm -fr %s", oldLogName.c_str(), oldLogName.c_str(), oldLogName.c_str());
		int ret = system(cmd);
	}
}