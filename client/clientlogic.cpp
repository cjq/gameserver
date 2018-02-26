#include "clientlogic.h"
#include "../public/log/log.h"

CClientLogic::CClientLogic()
{

}

CClientLogic::~CClientLogic()
{

}

int CClientLogic::onMessage(void* buf, int size, uint32_t sessionid)
{
	if (NULL == buf || size <= 0 || sessionid <= 0)
	{
		return 0;
	}
	MSG_BASE* cmd = (MSG_BASE*)buf;
	INFO("send %d-%d size=%d uid=%d", cmd->cmd1, cmd->cmd2, cmd->size, cmd->uid);
	static int index = 0;
	if (index++ < 10)
	{
		MSG_BASE cmd;
		cmd.cmd1 = 101 + index;
		cmd.cmd2 = 102 + index;
		cmd.gameid = 12 + index;
		cmd.uid = 102321 + index;
		cmd.flage = 0x8;
		cmd.flags = 0x8;
		cmd.size = sizeof(MSG_BASE);
		CServerMgr.send(sessionid, &cmd, cmd.size);
		INFO("send %d-%d size=%d uid=%d", cmd.cmd1, cmd.cmd2, cmd.size, cmd.uid);
	}

}

int CClientLogic::onConnect(uint32_t sessionid)
{
	MSG_BASE cmd;
	cmd.cmd1 = 101;
	cmd.cmd2 = 102;
	cmd.gameid = 12;
	cmd.uid = 102321;
	cmd.flage = 0x8;
	cmd.flags = 0x8;
	cmd.size = sizeof(MSG_BASE);
	CServerMgr.send(sessionid, &cmd, cmd.size);
	INFO("send %d-%d size=%d uid=%d", cmd.cmd1, cmd.cmd2, cmd.size, cmd.uid);
}

int CClientLogic::onTimeOut(uint8_t timerid, uint32_t param)
{

}

int CClientLogic::onSocketClose(uint32_t sessionid, int type)
{

}