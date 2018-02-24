#ifndef __PUBLIC_LOGICBASE_LOGICBASE_H__
#define __PUBLIC_LOGICBASE_LOGICBASE_H__

#include "../util/util.h"
#include "../socket/servermgr.h"

class CLogicBase
{
	friend class CServerManager;
public:
	/*
	* ����������Ϣ
	*/
	virtual int onMessage(void* buf, int size, uint32_t sessionid){}

	/*
	* �������ӶϿ�
	*/
	virtual int onSocketClose(uint32_t sessionid, int type){}

	/*
	* ����socket����
	*/
	virtual int onConnect(uint32_t sessionid){}

	/*
	* ����ʱ��ʱ��
	*/
	virtual int onTimeOut(uint8_t timerid, uint32_t param){}

	/*
	* ����http��Ϣ
	*/
	virtual int onHttpMessage(void* buf, int size, uint32_t sessionid){}
private:
	int _onMessage(void* buf, int size, uint32_t sessionid);

	int _onConnect(uint32_t sessionid);

	int _onTimeOut(uint8_t timerid, uint32_t param);

	int _onSocketClose(uint32_t sessionid, int type);

	void onLoginServer(void* buf, int size, uint32_t sessionid);

	void onUpdateServer(void* buf, int size, uint32_t sessionid);
};

#endif