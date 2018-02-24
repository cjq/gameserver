#include "gatelogic.h"
#include "../public/socket/servermgr.h"
#include "../public/log/log.h"

int main(int argc, char* argv[])
{
	ConfigMgr.parseConfig(argv[1]);
	ConfigMgr.printConfig();

	CLogMgr.init_log();

	CServerMgr.initServer(new CGateLogic());

	CServerMgr.createServer("0.0.0.0", 5888);

	CServerMgr.loop();

	return 0;
}
