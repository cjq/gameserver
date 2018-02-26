#include "gatelogic.h"
#include "../public/log/log.h"

CGateLogic::CGateLogic()
{

}

CGateLogic::~CGateLogic()
{

}

int CGateLogic::onConnect(uint32_t sessionid)
{

}

int CGateLogic::onSocketClose(uint32_t sessionid, int type)
{

}

int CGateLogic::onMessage(void* buf, int size, uint32_t sessionid)
{
	MSG_BASE* pMsg = (MSG_BASE*)buf;
	INFO("CGateLogic::onMessage");
	if (0 == pMsg->arg1)
	{
		INFO("c->s %d-%d size=%d sessionid=%d uid=%d", pMsg->cmd1, pMsg->cmd2,
			pMsg->size, sessionid, pMsg->uid);

		MSG_BASE* serverMsg = new MSG_BASE();
		serverMsg->cmd1 = 2;
		serverMsg->cmd2 = 4;
		serverMsg->gameid = 1;
		serverMsg->uid = 1000;
		serverMsg->flage = 0x8;
		serverMsg->flags = 0x8;
		serverMsg->size = sizeof(MSG_BASE);
		CServerMgr.send(sessionid, serverMsg, serverMsg->size);
	}
	else if (1 == pMsg->arg1)
	{
		INFO("s->c %d-%d size=%d sessionid=%d uid=%d", pMsg->cmd1, pMsg->cmd2,
			pMsg->size, sessionid, pMsg->uid);
	}

}

int CGateLogic::onTimeOut(uint8_t timerid, uint32_t param)
{
	
}