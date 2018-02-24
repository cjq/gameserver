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

	if (0 == pMsg->arg1)
	{
		INFO("c->s %d-%d size=%d sessionid=%d uid=%d", pMsg->cmd1, pMsg->cmd2,
			pMsg->size, sessionid, pMsg->uid);
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