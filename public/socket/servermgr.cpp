#include "servermgr.h"
#include "../log/log.h"

#define SOCKET_TYPE_CLIENT 1
#define SOCKET_TYPE_SERVER 2

CServerManager::CServerManager()
{
	m_connNum = 0;
	m_bRun = false;
	m_connNumConfig = 0;
	m_threadNum = 0;
	ZERO(m_bufTemp);
	ZERO(m_sendBufTemp);

	m_heartMsg.cmd1 = SCMD_HEART;
	m_heartMsg.cmd2 = 1;
	m_heartMsg.size = sizeof(m_heartMsg);

	int configValue = 0;
	getConfigInt("MAX_CON_NUM");
	printf("MAX_CON_NUM:%d\n", configValue);
	int allConNum = configValue + 10;
	m_connNumConfig = configValue;
	getConfigInt("THREAD_NUM");
	printf("THREAD_NUM:%d\n", configValue);
	int threadNum = configValue;
	m_threadNum = configValue;
	getConfigInt("BUF_SIZE");
	printf("BUF_SIZE:%d\n", configValue);
	int maxBufSize = configValue;

	m_pEventInfo = new TEventInfo[allConNum];
	if (!m_pEventInfo)
	{
		printf("alloc eventinfo error\n");
		return;
	}

	CSocket* pSocket = NULL;
	for (int i = 0; i < allConNum; i++)
	{
		pSocket = new CSocket();
		if (!pSocket)
		{
			printf("new psocket error \n");
		}
		m_socketList.push_back(pSocket);
	}

	CBuffer* pMessage = NULL;
	uint32_t tsize = allConNum * sizeof(TSocketInfo) / threadNum;
	for (int k = 0; k < threadNum; ++k)
	{
		pMessage = new CBuffer(tsize);
		if (!pMessage)
		{
			printf("create message size:%d, k:%d error\n", tsize, k);
		}
		m_readQueue.push_back(pMessage);
		INFO("init message tsize:%d suc", tsize);
	}

	tsize = maxBufSize * allConNum / threadNum;
	if (tsize > 1024 * 1024 * 1024)
	{
		tsize = 1024 * 1024 * 1024; // 大小不超过一个G
	}

	pMessage = NULL;
	for (int j = 0; j < threadNum; ++j)
	{
		pMessage = new CBuffer(tsize);
		if (!pMessage)
		{
			INFO("create data message j:%d ,size:%d error", j, tsize);
		}
		m_dataQueue.push_back(pMessage);
	}

	//init epoll
	m_epollFD = epoll_create(allConNum);
	INFO("init epoll ok, fd:%d", m_epollFD);

	//信号忽略，避免对端关闭，本端连续两次写就会产生SIGPIPE信号导致退出
	signal(SIGPIPE, SIG_IGN);
}

CServerManager::~CServerManager()
{

}

CServerManager& CServerManager::getInstance()
{
	static CServerManager mgr;
	return mgr;
}

void CServerManager::initServer(CLogicBase* pServer)
{
	m_pServer = pServer;
}

int CServerManager::createServer(const char* hostname, uint32_t port)
{
	if (port <= 0)
	{
		return 0;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	CSocketOpts::setNoBlocking(fd);

	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	if (!hostname || 0 == strcmp(hostname, "0.0.0.0"))
	{
		serveraddr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		serveraddr.sin_addr.s_addr = inet_addr(hostname);
	}

	if (!CSocketOpts::initSocket(fd))
	{
		return -1;
	}

	if (bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
	{
		ERROR("bind port:%d error", port);
		return -1;
	}

	if (listen(fd, 1000) < 0)
	{
		ERROR("listen fd:%d error", fd);
		return -1;
	}

	INFO("server listen %d suc fd:%d", port, fd);

	TEventInfo* pInfo = &m_pEventInfo[fd];
	pInfo->fd = fd;
	pInfo->type = SOCKET_TYPE_SERVER;
	pInfo->port = port;

	struct epoll_event tmpev;
	memset(&tmpev, 0, sizeof(struct epoll_event));
	tmpev.data.ptr = pInfo;
	tmpev.events = EPOLLIN | EPOLLET;
	epoll_ctl(m_epollFD, EPOLL_CTL_ADD, fd, &tmpev);
	
	return 0;
}

TClientInfo* CServerManager::getClientInfo(int id)
{
	ClientInfoMap::iterator it = m_clientMap.find(id);
	if (it != m_clientMap.end())
	{
		return &it->second;
	}
	return NULL;
}

int CServerManager::createClient(int id, const char* hostname, uint32_t port)
{
	if (id <= 0 || NULL == hostname || port <= 0)
	{
		return 0;
	}

	if (NULL != getClientInfo(id))
	{
		return 0;
	}

	TClientInfo info;
	info.id = id;
	info.port = port;
	memcpy(info.hostname, hostname, strlen(hostname) + 1);
	m_clientMap.insert(ClientInfoMap::value_type(id, info));
	
	return 0;
}

int CServerManager::send(uint32_t sessionid, void* buf, int size)
{
	Session s;
	s.session = sessionid;
	if (s.data.fd <= 0 || s.data.fd >= m_connNumConfig)
	{
		return 0;
	}
	CSocket* pSocket = m_socketList[s.data.fd];
	if (NULL == pSocket)
	{
		return 0;
	}
	pSocket->send(buf, size);
	pSocket->sendData();

	MSG_BASE* pHead = (MSG_BASE*)buf;
	INFO("send %d-%d size=%d uid=%d sessionid=%d arg3=%d fx=%d",
		pHead->cmd1, pHead->cmd2, pHead->size, pHead->uid, sessionid, pHead->arg3, pHead->arg1);

	return 0;
}

int CServerManager::addMessage(TSocketInfo* pInfo)
{
	static int index = 0;
	CSafeLock lk(&m_readMsgLock);
	int ret = m_dataQueue[index]->write((char*)pInfo, pInfo->size);
	index = ++index % m_threadNum;
	return 0;
}

void CServerManager::closeSocket(uint32_t sessionid)
{
	if (sessionid <= 0)
	{
		return;
	}

	Session s;
	s.session = sessionid;
	int fd = s.data.fd;
	CSocket* pSocket = m_socketList[fd];
	if (NULL == pSocket)
	{
		return;
	}

	if (0 != pSocket->getID())
	{
		//处理断线重连定时器
	}

	if (m_pServer)
	{
		m_pServer->_onSocketClose(sessionid, pSocket->getID());
	}

	m_connNum--;

	INFO("close socket fd:%d, connNum:%d", fd, m_connNum);
	pSocket->clean();
		
	struct epoll_event tmpev;
	ZERO(tmpev);
	tmpev.data.fd = fd;
	epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, &tmpev);

	CSocketOpts::closeSocket(fd);
}

void* CServerManager::acceptThread(void* p)
{
	if (NULL == p)
	{
		return 0;
	}

	CServerManager* pMgr = (CServerManager*)p;
	pMgr->_acceptThread();
	return 0;
}

void CServerManager::_acceptThread()
{
	INFO("accept thread is running");
	struct sockaddr_in clientaddr;
	ZERO(clientaddr);
	struct epoll_event events[1000];
	ZERO(events);

	int fdNum = 0, newFd = 0;
	socklen_t clientLen = sizeof(clientaddr);

	struct epoll_event tmpEvent;

	TSocketInfo socketReadyInfo;
	int headLen = sizeof(socketReadyInfo);
	int threadIndex = 0;

	while (m_bRun)
	{
		fdNum = epoll_wait(m_epollFD, events, 1000, -1);
		for (int i = 0; i < fdNum; ++i)
		{
			TEventInfo* pInfo = (TEventInfo*)events[i].data.ptr;
			if (NULL == pInfo)
			{
				continue;
			}
			
			if (SOCKET_TYPE_SERVER == pInfo->type)
			{
				// 收到连接信息
				while (true)
				{
					newFd = accept(pInfo->fd, (struct sockaddr*)&clientaddr, &clientLen);
					if (newFd < 0)
					{
						close(newFd);
						break;
					}

					if (newFd >= m_connNumConfig)
					{
						close(newFd);
						continue;
					}

					if (!CSocketOpts::initSocket(newFd))
					{
						close(newFd);
						continue;
					}

					m_connNum++;
					CSocket* pSocket = m_socketList[newFd];
					if (NULL == pSocket)
					{
						continue;
					}
					pSocket->init(newFd);
					INFO("Accept new client fd : %d session:%d", newFd, pSocket->getSession());

					TEventInfo* pInfoEx = &m_pEventInfo[newFd];
					pInfoEx->fd = newFd;
					pInfoEx->ptr1 = pInfo->ptr1;
					pInfoEx->ptr2 = pSocket;
					pInfoEx->type = SOCKET_TYPE_CLIENT;
					ZERO(tmpEvent);
					tmpEvent.data.ptr = pInfoEx;
					tmpEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
					epoll_ctl(m_epollFD, EPOLL_CTL_ADD, newFd, &tmpEvent);
				}

			}
			else if (SOCKET_TYPE_CLIENT == pInfo->type)
			{
				// 已经建立连接socket进行通信
				if (events[i].events & EPOLLIN)
				{
					INFO("read fd=%d c->s info", pInfo->fd);
					socketReadyInfo.fd = pInfo->fd;
					socketReadyInfo.type = 1;
					m_readQueue[threadIndex++]->write((char*)&socketReadyInfo, headLen);
					threadIndex %= m_threadNum;
				}

				if (events[i].events & EPOLLOUT)
				{
					INFO("write fd=%d c->s info", pInfo->fd);
					socketReadyInfo.fd = pInfo->fd;
					socketReadyInfo.type = 2;
					m_readQueue[threadIndex++]->write((char*)&socketReadyInfo, headLen);
					threadIndex %= m_threadNum;
				}
			}
			else
			{
				ERROR("error fd type fd=%d type=%d event=%d", pInfo->fd, pInfo->type, events[i].events);
			}
		}

	}

}

void* CServerManager::messageThread(void* p)
{
	if (NULL == p)
	{
		return 0;
	}

	static int id = 0;
	CServerManager* pMgr = (CServerManager*)p;
	pMgr->_messageThread(id);
	return 0;
}

void CServerManager::_messageThread(int id)
{
	char bHttpData = 0;
	int ret = 0;
	char buf[MAX_MSG_LEN];
	ZERO(buf);

	CSocket* pSocket = NULL;
	INFO("message thread %d running", id);
	while (m_bRun)
	{
		try
		{
			ret = m_dataQueue[id]->read(buf, MAX_MSG_LEN);
			if (ret <= 0)
			{
				continue;
			}

			TSocketInfo* pInfo = (TSocketInfo*)buf;
			if (0 == pInfo->type)
			{
				if (pInfo->fd > 0 && pInfo->fd < m_connNumConfig)
				{
					pSocket = m_socketList[pInfo->fd];
					if (NULL == pSocket)
					{
						continue;
					}
					memset(buf, 0, ret);

					while (true)
					{
						bHttpData = 0;
						ret = pSocket->getData((char*)buf, MAX_MSG_LEN, bHttpData);
						INFO("ret fd=%d ret=%d", pInfo->fd, ret);
						if (ret < 0)
						{
							closeSocket(pSocket->getSession());
							break;
						}

						if (0 == ret)
						{
							break;
						}

						if (0 == bHttpData)
						{
							MSG_BASE* pHead = (MSG_BASE*)buf;
							INFO("recv %d-%d size=%d uid=%d sessionid=%d arg3=%d fx=%d gameid=%d serverno=%d",
								pHead->cmd1, pHead->cmd2, pHead->size, pHead->uid, pSocket->getSession(),
								pHead->arg3, pHead->arg1, pHead->gameid, pHead->serverNo);
							m_pServer->_onMessage(buf, ret, pSocket->getSession());
						}
						else
						{
							m_pServer->onHttpMessage(buf, ret, pSocket->getSession());
						}

						memset(buf, 0, ret);
					}
				}
				else
				{
					INFO("recv other info");
					m_pServer->_onMessage((char*)(pInfo + 1), ret - sizeof(TSocketInfo), 0);
				}
			}
			else if (1 == pInfo->type)
			{
				//定时器消息
			}
			else
			{
				ERROR("message thread recv %d type error", pInfo->type);
			}
		}
		catch (...)
		{
			ERROR("catch message thread error");
		}
	}
}

void* CServerManager::readThread(void* p)
{
	if (NULL == p)
	{
		return 0;
	}

	static int id = 0;
	CServerManager* pMgr = (CServerManager*)p;
	pMgr->_readThread(id++);
	return 0;
}

void CServerManager::_readThread(int id)
{
	INFO("read thread id:%d is running", id);

	int fd = -1;
	int readLen = 0;
	int bufLen = sizeof(TSocketInfo);
	TSocketInfo socketInfo;
	CSocket* pSocket = NULL;

	while (m_bRun)
	{
		ZERO(socketInfo);
		m_readQueue[id]->read((char*)&socketInfo, bufLen);

		if (socketInfo.fd <= 0 || socketInfo.fd >= m_connNumConfig)
		{
			INFO("readthread fd=%d error", socketInfo.fd);
			continue;
		}

		pSocket = m_socketList[socketInfo.fd];
		if (NULL == pSocket || pSocket->getFD() <= 0)
		{
			INFO("readthread fd=%d socket error", socketInfo.fd);
			continue;
		}

		if (1 == socketInfo.type)
		{
			readLen = pSocket->recv();
			if (readLen < 0)
			{
				closeSocket(pSocket->getSession());
			}
			else
			{
				socketInfo.type = 0;
				addMessage(&socketInfo);
			}
		}
		else if (2 == socketInfo.type)
		{
			readLen = pSocket->sendData();
			INFO("send datalen:%d", readLen);
			if (readLen < 0)
			{
				closeSocket(pSocket->getSession());
			}
		}
		else
		{
			ERROR("read thread recv type:%d error", socketInfo.type);
		}

	}

}

bool CServerManager::loop()
{
	if (m_bRun)
	{
		return false;
	}

	m_bRun = true;

	for (int i = 0; i < m_threadNum; i++)
	{
		pthread_t threadid;
		if (pthread_create(&threadid, NULL, &CServerManager::readThread, this) < 0)
		{
			ERROR("create readthread %d error", i);
			exit(1);
		}
	}

	for (int i = 0; i < m_threadNum; i++)
	{
		pthread_t threadid;
		if (pthread_create(&threadid, NULL, &CServerManager::messageThread, this) < 0)
		{
			ERROR("create messagethread %d error", i);
			exit(1);
		}
	}

	if (pthread_create(&m_threadId, NULL, &CServerManager::acceptThread, this) < 0)
	{
		ERROR("create acceptthrad error");
		exit(1);
	}

	for (ClientInfoMap::iterator it = m_clientMap.begin(); it != m_clientMap.end(); ++it)
	{
		connentServer(&it->second);
	}

	INFO("server start now.....");
	pthread_join(m_threadId, NULL);
	return true;
}

bool CServerManager::connentServer(TClientInfo* pClient)
{
	if (NULL == pClient)
	{
		return false;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd <= 0)
	{
		return false;
	}

	CSocket* pSocket = m_socketList[fd];
	if (NULL == pSocket)
	{
		return false;
	}

	pSocket->init(fd, pClient->id);

	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(pClient->port);
	serveraddr.sin_addr.s_addr = inet_addr(pClient->hostname);

	m_connNum++;
	if (0 != connect(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)))
	{
		ERROR("connect to server %s:%d fd=%d fail.%s", pClient->hostname, pClient->port, fd, strerror(errno));
		closeSocket(pSocket->getSession());
		return false;
	}

	CSocketOpts::initSocket(fd);

	INFO("connect to %s:%d fd=%d ok", pClient->hostname, pClient->port, fd);

	TEventInfo* pEventInfo = &m_pEventInfo[fd];
	pEventInfo->fd = fd;
	pEventInfo->ptr2 = pSocket;
	pEventInfo->type = SOCKET_TYPE_CLIENT;

	struct epoll_event tmpev;
	memset(&tmpev, 0, sizeof(struct epoll_event));
	tmpev.data.ptr = pEventInfo;
	tmpev.events = EPOLLIN | EPOLLOUT | EPOLLET;

	epoll_ctl(m_epollFD, EPOLL_CTL_ADD, fd, &tmpev);

	if (m_pServer)
	{
		m_pServer->_onConnect(pSocket->getSession());
	}

	pClient->sessionid = pSocket->getSession();
	
	return true;
}