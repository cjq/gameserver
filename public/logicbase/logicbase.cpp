#include "logicbase.h"

int CLogicBase::_onMessage(void* buf, int size, uint32_t sessionid)
{
	onMessage(buf, size, sessionid);
}

int CLogicBase::_onConnect(uint32_t sessionid)
{
	onConnect(sessionid);
}

int CLogicBase::_onSocketClose(uint32_t sessionid, int type)
{
	onSocketClose(sessionid, type);
}

int CLogicBase::_onTimeOut(uint8_t timerid, uint32_t param)
{
	onTimeOut(timerid, param);
}

void CLogicBase::onLoginServer(void* buf, int size, uint32_t sessionid)
{

}

void CLogicBase::onUpdateServer(void* buf, int size, uint32_t sessionid)
{

}