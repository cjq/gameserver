#ifndef __PUBLIC_SERVERMGR_H__
#define __PUBLIC_SERVERMGR_H__

#include "socketopt.h"
#include "socket.h"
#include "../util/util.h"
#include "../buffer/buffer.h"
#include "../structmsg/structmsg.h"
#include "../structmsg/defineinfo.h"
#include "../logicbase/logicbase.h"

#include <map>
#include <vector>
#include <pthread.h>
#include <signal.h>

typedef std::map<int , TClientInfo> ClientInfoMap;

class CLogicBase;

class CServerManager
{
public:
	static CServerManager& getInstance();

	/*
	* �����߼��������
	*/
	void initServer(CLogicBase* pServer);

	/*
	* ����һ����������������,��Ϣ���͵�pServer����
	*/
	int createServer(const char* hostname, uint32_t port);

	/*
	* ����һ���ͻ��ˣ���Ϣ���͵�pClient�����Ͽ������һ��ʱ��������
	*/
	int createClient(int id, const char* hostname, uint32_t port);

	/*
	* ������Ϣ��session
	*/
	int send(uint32_t sessionid, void* buf, int size);


	int addMessage(TSocketInfo* pInfo);


	void closeSocket(uint32_t sessionid);

	bool loop();

	/*
	* ��ȡ�����̵��ͻ���������Ϣ
	*/
	TClientInfo* getClientInfo(int id);

	/*
	* ���ӷ�����
	*/
	bool connentServer(TClientInfo* pClient);
private:
	CServerManager();

	~CServerManager();

	static void* readThread(void* p);

	static void* acceptThread(void* p);

	static void* messageThread(void* p);

	void _acceptThread();

	void _readThread(int id);

	void _messageThread(int id);

private:
	int				m_epollFD;
	bool			m_bRun;
	DataQueueList	m_readQueue;
	DataQueueList   m_dataQueue;
	CSocketList		m_socketList;
	TEventInfo*		m_pEventInfo;
	ClientInfoMap	m_clientMap;
	CGameLock		m_readMsgLock;
	CGameLock		m_sendMsgLock;
	int				m_connNum;
	pthread_t		m_threadId;
	char			m_bufTemp[MAX_MSG_LEN];
	char			m_sendBufTemp[MAX_MSG_LEN];
	MSG_BASE		m_heartMsg;
	CLogicBase*		m_pServer;
	int				m_connNumConfig; //���õ����������
	int				m_threadNum;
};

#define CServerMgr CServerManager::getInstance()

#endif