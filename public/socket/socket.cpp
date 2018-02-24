#include "socket.h"
#include "socketopt.h"

#define HTTPHEADLEN 4

CSocket::CSocket(int bufSize): m_readBuf(bufSize), m_writeBuf(bufSize)
{
	ZERO(m_session);
	m_readBuf.clear();
	m_writeBuf.clear();
	m_fd = -1;
	m_accpetTime = 0;
	m_lastDataTime = 0;
	m_readAllDataLen = 0;
	m_writeAllDataLen = 0;
}

CSocket::~CSocket()
{

}

void CSocket::init(int fd, int id /* = 0 */)
{
	clean();
	m_fd = fd;
	m_id = id;
	m_session.data.id = id;
	m_session.data.fd = fd;
}

void CSocket::clean()
{
	if (m_fd > 0)
	{
		CSocketOpts::closeSocket(m_fd);
	}
	ZERO(m_session);

	m_readBuf.clear();
	m_writeBuf.clear();

	m_fd = -1;
	m_accpetTime = 0;
	m_lastDataTime = 0;
	m_readAllDataLen = 0;
	m_writeAllDataLen = 0;
}

int CSocket::recv()
{
	LOCK(m_readLock);
	int readAllLen = 0;
	int readLen = 0;
	int canWriteLen = 0;

	while(true)
	{
		while ((canWriteLen = m_readBuf.getCanWriteLen()) <= 0)
		{
			if (!m_readBuf.reallocMem())
			{
				return 0;
			}
		}

		readLen = read(m_fd, m_readBuf.getWritePtr(), canWriteLen);

		if (readLen < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}

		if (0 == readLen)
		{
			return -1;
		}

		m_readBuf.setCurWritePos(readLen);
		readAllLen += readLen;
		m_lastDataTime = time(NULL);
		m_readAllDataLen += readLen;

		if (readLen < canWriteLen)
		{
			break;
		}
	}
	return readAllLen;
}

int CSocket::send(void* buf, int size)
{
	if (NULL == buf || size <= 0 || m_fd < 0)
	{
		return 0;
	}
	LOCK(m_writeLock);
	return m_writeBuf.write(buf, size);
}

int CSocket::getData(void *desc, int maxSize, char& bHttp)
{
	if (NULL == desc || maxSize <= 0)
	{
		return 0;
	}

	LOCK(m_readLock);

	int msgHeadLen = sizeof(MSG_BASE);
	int readLen = 0;

	// 先尝试去读http格式的数据，如果是的话直接读出，不是的话按照正常格式读取
	readLen = getHttpData((char*)desc, maxSize);
	if (readLen > 0)
	{
		bHttp = 1;
		return readLen;
	}

	// 读取消息头,如果出错，一位一位往后读，进行纠错处理
	MSG_BASE* pMsg = NULL;
	while (true)
	{
		readLen = m_readBuf.read(desc, msgHeadLen);
		if (readLen != msgHeadLen)
		{
			return 0;
		}

		pMsg = (MSG_BASE*)desc;
		if (0x8 == pMsg->flags && 0x8 == pMsg->flage && pMsg->size > 0 && pMsg->size < maxSize)
		{
			break;
		}
		m_readBuf.setCurReadPos(1);
	}

	if (pMsg->size > 0 && pMsg->size > maxSize)
	{
		printf("消息长度错误，消息长度：%d, 最大长度:%d\n", pMsg->size, maxSize);
		return 0;
	}

	readLen = m_readBuf.read(desc, pMsg->size);
	if (readLen != pMsg->size)
	{
		return 0;
	}
	m_readBuf.setCurReadPos(pMsg->size);
	return pMsg->size;
}

int CSocket::sendData()
{
	LOCK(m_writeLock);
	int bufWriteLen = 0;
	int sendLen = 0;
	while ((bufWriteLen = m_writeBuf.getCanReadLen()) > 0)
	{
		sendLen = write(m_fd, m_writeBuf.getReadPtr(), bufWriteLen);
		if (sendLen > 0)
		{
			m_writeBuf.setCurReadPos(sendLen);
			m_writeAllDataLen += sendLen;
		}
		else
		{
			if (errno == EINTR)
			{
				continue;
			}
			else if (errno == EAGAIN)
			{
				break;
			}
			else
			{
				return 0;
			}
		}

		if (sendLen < bufWriteLen)
		{
			break;
		}
	}
	return 0;
}

int CSocket::getHttpData(char *pBuf, int maxBufSize)
{
	if (m_readBuf.getDataLen() < HTTPHEADLEN)
	{
		return 0;
	}

	int readLen = m_readBuf.read(pBuf, HTTPHEADLEN);
	if (readLen < HTTPHEADLEN)
	{
		return 0;
	}

	if (0 != strncmp(pBuf, "POST", 4) || 0 != strncmp(pBuf, "GET ", 4))
	{
		return 0;
	}

	readLen = m_readBuf.getDataLen();
	if (readLen > maxBufSize)
	{
		readLen = maxBufSize;
	}

	readLen = m_readBuf.read(pBuf, readLen);
	m_readBuf.setCurReadPos(readLen);
	return readLen;
}

