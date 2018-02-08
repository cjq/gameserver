#include <iostream>
#include <string>
#include "../public/config/config.h"
#include "../public/log/log.h"

int main(int argc, char* argv[])
{
	ConfigMgr.parseConfig(argv[1]);
	ConfigMgr.printConfig();
	std::string str;
	ConfigMgr.getConfigInfo("FILE_SIZE", str);
	std::cout<<"ConfigMgr filesize:" << str <<std::endl;

	CLogMgr.init_log();

	return 0;
}