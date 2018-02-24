#ifndef __PUBLIC_CONFIG_CONFIG_H__
#define __PUBLIC_CONFIG_CONFIG_H__

#include <iostream>
#include <map>
#include <string.h>
#include <vector>

class CConfigMgr
{
public:
	static CConfigMgr& getInstance(); 

	void parseConfig(const char* filename);
	
	void getConfigInfo(const std::string findstr, std::string& dststr);

	static void split(const std::string& str, std::vector<std::string>& result, const std::string& c);

	void printConfig();
private:
	CConfigMgr();

	~CConfigMgr();

private:
	std::map<std::string, std::string> m_strConfig;
};


#define ConfigMgr CConfigMgr::getInstance()


#endif