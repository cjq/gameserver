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
	* ����socket
	*/
	static int createSocketFD();

	/*
	* �����ļ����������շ�Ϊ������
	*/
	static bool setNoBlocking(int fd);

	/*
	* �����ļ����������շ�Ϊ����
	*/
	static bool setBlocking(int fd);

	/*
	* �����ļ���������Nodelayѡ��ر�nagle�㷨
	*/
	static bool openBufferNoDelay(int fd);

	/*
	* �ر��ļ���������nodelayѡ�����nagle�㷨
	*/
	static bool closeBufferNoDelay(int fd);

	/*
	* �����ļ��������յ��ĳ�ʱʱ��
	*/
	static bool setFdTimeout(int fd, uint32_t timeout);

	/*
	* �ر��ļ�������
	*/
	static void closeSocket(int fd);

	/*
	* �����ļ���������SO_REUSEADDRѡ��
	*/
	static void setReuseAddr(int fd);

	/*
	* �����ļ��������Ľ��ջ�������С
	*/
	static bool setRecvBufferSize(int fd, uint32_t size);

	/*
	* �����ļ��������ķ��ͻ�������С
	*/
	static bool setSendBufferSize(int fd, uint32_t size);

	/*
	* ��ʼ��socket
	*/
	static bool initSocket(int fd);
};

#endif