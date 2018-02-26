#ifndef __CLIENT_CLIENTLOGIC_H__
#define __CLIENT_CLIENTLOGIC_H__

#include "../public/logicbase/logicbase.h"

class CClientLogic : public CLogicBase
{
public:
	CClientLogic();

	~CClientLogic();

	virtual int onMessage(void* buf, int size, uint32_t sessionid);

	virtual int onConnect(uint32_t sessionid);

	virtual int onSocketClose(uint32_t sessionid, int type);

	virtual int onTimeOut(uint8_t timerid, uint32_t param);
};

#endif