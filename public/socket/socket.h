#ifndef __PUBLIC_SOCKET_H__
#define __PUBLIC_SOCKET_H__

#include "../structmsg/msgbase.h"
#include "../buffer/circularbuffer.h"
#include "../lock/lock.h"
#include "../config/config.h"

#include <vector>

class CSocket
{
public:
	CSocket(int bufSize = 1024 * 1024);

	~CSocket();

	/*
	* 初始化信息
	*/
	void init(int fd, int id = 0);

	/*
	* 清理数据信息
	*/
	void clean();

	/*
	* 从socket读取信息并写到缓存中
	*/
	int recv();

	/*
	* 发送的数据写到缓存列表中
	*/
	int send(void* buf, int size);

	/*
	* 从缓存中读取数据
	*/
	int getData(void *desc, int maxSize, char& bHttp);

	/*
	* 写入到缓存中的数据发送到socket
	*/
	int sendData();

	/*
	* 读取http数据
	*/
	int getHttpData(char *pBuf, int maxBufSize);

	uint32_t getSession() {return m_session.session;}

	int getFD() {return m_fd;}

	int getID() {return m_id;}

	void setID(int id) {m_id = id; m_session.data.id = id;}

private:
	Session				m_session;
	int					m_id;
	int					m_fd;

	CCircularBuffer		m_readBuf;
	CGameLock			m_readLock;

	CCircularBuffer		m_writeBuf;
	CGameLock			m_writeLock;

	int					m_accpetTime;
	int					m_lastDataTime;
	uint32_t			m_readAllDataLen;
	uint32_t			m_writeAllDataLen;
};

typedef std::vector<CSocket*> CSocketList;

#endif