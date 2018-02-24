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
	* �������״̬
	*/
	void clear();

	/*
	* д���ݵ�������
	* @param buf ������ʼ��ַ
	* @param len ���ݵĳ���
	* @return д�����ݳ���
	*/
	int write(void* buf, int len);

	/*
	* �ӻ������ж�����
	* @param buf ���ݵ���ʼ��ַ
	* @param len ��ȡ�ĳ���
	* @return ��ȡ�ĳ���
	*/
	int read(void* buf, int len);

	/*
	* ��ȡ��д��ĳ���
	* @return ��д�ĳ���
	*/
	int getCanWriteLen();

	/*
	* ��ȡ�ܶ�ȡ�ĳ���
	* @return ��ȡ����󳤶�
	*/
	int getCanReadLen();

	/*
	* ���������ݳ��Ȳ�������������һ��
	*/
	bool reallocMem();

	/*
	* ���õ�ǰд���Ļ�����λ��
	*/
	void setCurWritePos(int len);

	/*
	* ���õ�ǰ�����Ļ�����λ��
	**/
	void setCurReadPos(int len);

	/*
	* ��ȡд��λ��
	*/
	char* getWritePtr();

	/*
	* ��ȡ����λ��
	*/
	char* getReadPtr();

	/*
	* ��ȡ���ݳ���
	*/
	int getDataLen() {return m_nDataSize;}

private:
	char* m_buf;		// ����������
	int m_nReadPos;		// ��ȡ���ݵ�λ��
	int m_nWritePos;	// д���ݵ�λ��
	int m_nDataSize;	// ����������Ч���ݳ���
	int m_nSize;		// ����������
};

#endif