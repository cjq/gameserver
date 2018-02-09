#ifndef __PUBLIC_SOCKET_OPT_H__
#define __PUBLIC_SOCKET_OPT_H__

#include "../util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <unistd.h>

class CSocketOpts
{
public:
	/*
	* 创建socket
	*/
	static int createSocketFD();

	/*
	* 设置文件描述符的收发为非阻塞
	*/
	static bool setNoBlocking(int fd);

	/*
	* 设置文件描述符的收发为阻塞
	*/
	static bool setBlocking(int fd);

	/*
	* 开启文件描述符的Nodelay选项，关闭nagle算法
	*/
	static bool openBufferNoDelay(int fd);

	/*
	* 关闭文件描述符的nodelay选项，开启nagle算法
	*/
	static bool closeBufferNoDelay(int fd);

	/*
	* 设置文件描述符收到的超时时间
	*/
	static bool setFdTimeout(int fd, uint32_t timeout);

	/*
	* 关闭文件描述符
	*/
	static void closeSocket(int fd);

	/*
	* 设置文件描述符的SO_REUSEADDR选项
	*/
	static void setReuseAddr(int fd);

	/*
	* 设置文件描述符的接收缓冲区大小
	*/
	static bool setRecvBufferSize(int fd, uint32_t size);

	/*
	* 设置文件描述符的发送缓冲区大小
	*/
	static bool setSendBufferSize(int fd, uint32_t size);
};

#endif