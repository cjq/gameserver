#include "socketopt.h"

int CSocketOpts::createSocketFD()
{
	return socket(AF_INET, SOCK_STREAM, 0);
}

bool CSocketOpts::setNoBlocking(int fd)
{
	uint32_t value = 1;
	return 0 == ioctl(fd, FIONBIO, &value);
}

bool CSocketOpts::setBlocking(int fd)
{
	uint32_t value = 0;
	return 0 == ioctl(fd, FIONBIO, &value);
}

bool CSocketOpts::openBufferNoDelay(int fd)
{
	uint32_t value = 1;
	return 0 == setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
}

bool CSocketOpts::closeBufferNoDelay(int fd)
{
	uint32_t value = 0;
	return 0 == setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
}

void CSocketOpts::closeSocket(int fd)
{
	shutdown(fd, SHUT_RDWR);
	::close(fd);
}

bool CSocketOpts::setFdTimeout(int fd, uint32_t timeout)
{
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	if (0 != setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, (socklen_t)sizeof(tv)))
	{
		return false;
	}

	return 0 == setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, (socklen_t)sizeof(tv));
}

void CSocketOpts::setReuseAddr(int fd)
{
	uint32_t value = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&value, sizeof(value));
}

bool CSocketOpts::setRecvBufferSize(int fd, uint32_t size)
{
	return 0 == setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size));
}

bool CSocketOpts::setSendBufferSize(int fd, uint32_t size)
{
	return 0 == setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size));
}