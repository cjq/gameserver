#ifndef __GATE_GATELOGIC_H__
#define __GATE_GATELOGIC_H__

#include "../public/logicbase/logicbase.h"
#include "../public/lock/lock.h"

class CGateLogic : public CLogicBase
{
public:
	CGateLogic();

	~CGateLogic();

	virtual int onMessage(void* buf, int size, uint32_t sessionid);

	virtual int onTimeOut(uint8_t timerid, uint32_t param);

	virtual int onSocketClose(uint32_t sessionid, int type);

	virtual int onConnect(uint32_t sessionid);

private:
	CGameLock m_lock;
};

#endif