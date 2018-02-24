#ifndef __PUBLIC_BUFFER_CIRCULARBUFFER_H__
#define __PUBLIC_BUFFER_CIRCULARBUFFER_H__

#include <iostream>
#include <string.h>
#include <stdio.h>

class CCircularBuffer
{
public:
	CCircularBuffer(int size);

	~CCircularBuffer();

	/*
	* 清除数据状态
	*/
	void clear();

	/*
	* 写数据到缓冲区
	* @param buf 数据起始地址
	* @param len 数据的长度
	* @return 写入数据长度
	*/
	int write(void* buf, int len);

	/*
	* 从缓冲区中读数据
	* @param buf 数据的起始地址
	* @param len 读取的长度
	* @return 读取的长度
	*/
	int read(void* buf, int len);

	/*
	* 获取能写入的长度
	* @return 能写的长度
	*/
	int getCanWriteLen();

	/*
	* 获取能读取的长度
	* @return 读取的最大长度
	*/
	int getCanReadLen();

	/*
	* 缓冲区数据长度不够，重新申请一个
	*/
	bool reallocMem();

	/*
	* 设置当前写到的缓冲区位置
	*/
	void setCurWritePos(int len);

	/*
	* 设置当前读到的缓冲区位置
	**/
	void setCurReadPos(int len);

	/*
	* 获取写的位置
	*/
	char* getWritePtr();

	/*
	* 获取读的位置
	*/
	char* getReadPtr();

	/*
	* 获取数据长度
	*/
	int getDataLen() {return m_nDataSize;}

private:
	char* m_buf;		// 缓冲区数据
	int m_nReadPos;		// 读取数据的位置
	int m_nWritePos;	// 写数据的位置
	int m_nDataSize;	// 缓冲区中有效数据长度
	int m_nSize;		// 缓冲区长度
};

#endif