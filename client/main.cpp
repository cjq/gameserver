#include "clientlogic.h"
#include "../public/log/log.h"

int main(int argc, char* argv[])
{
	ConfigMgr.parseConfig(argv[1]);
	ConfigMgr.printConfig();

	CLogMgr.init_log();

	CServerMgr.initServer(new CClientLogic());

	CServerMgr.createClient(1000, "192.168.5.201", 5888);

	CServerMgr.loop();

	return 0;
}