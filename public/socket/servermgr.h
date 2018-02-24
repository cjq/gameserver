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
	* 设置逻辑处理对象
	*/
	void initServer(CLogicBase* pServer);

	/*
	* 创建一个服务器，并监听,消息发送到pServer处理
	*/
	int createServer(const char* hostname, uint32_t port);

	/*
	* 创建一个客户端，消息发送到pClient处理，断开后会在一定时间内重连
	*/
	int createClient(int id, const char* hostname, uint32_t port);

	/*
	* 发送消息到session
	*/
	int send(uint32_t sessionid, void* buf, int size);


	int addMessage(TSocketInfo* pInfo);


	void closeSocket(uint32_t sessionid);

	bool loop();

	/*
	* 获取本进程当客户端连接信息
	*/
	TClientInfo* getClientInfo(int id);

	/*
	* 连接服务器
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
	int				m_connNumConfig; //配置的最大连接数
	int				m_threadNum;
};

#define CServerMgr CServerManager::getInstance()

#endif