#include "config.h"
#include <fstream>

CConfigMgr::CConfigMgr()
{

}

CConfigMgr::~CConfigMgr()
{

}

void CConfigMgr::split(const std::string& str, std::vector<std::string>& result, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = str.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		result.push_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = str.find(c, pos1);
	}

	if (pos1 != str.length())
	{
		result.push_back(str.substr(pos1));
	}
}

CConfigMgr& CConfigMgr::getInstance()
{
	static CConfigMgr instance;
	return instance;
}

void CConfigMgr::parseConfig(const char* filename)
{
	if (NULL == filename)
	{
		printf("CConfigMgr::parseConfig filename error");
		return;
	}

	std::ifstream infile;
	infile.open(filename);

	std::string str;
	std::vector<std::string> vec;
	while(getline(infile, str))
	{
		split(str, vec, "=");
		if (vec.size() >= 2)
		{
			m_strConfig.insert(make_pair(vec[0], vec[1]));
		}
		vec.clear();
	}

	infile.close();
}

void CConfigMgr::getConfigInfo(const std::string findstr, std::string& dststr)
{
	std::map<std::string, std::string>::const_iterator it = m_strConfig.find(findstr);
	if (it != m_strConfig.end())
	{
		dststr = it->second;
	}
}

void CConfigMgr::printConfig()
{
	std::map<std::string, std::string>::const_iterator it = m_strConfig.begin();
	for (; it != m_strConfig.end(); ++it)
	{
		printf("%s = %s\n", it->first.c_str(), it->second.c_str());
	}
}