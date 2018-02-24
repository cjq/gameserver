#ifndef __PUBLIC_STRUCTMSG_H__
#define __PUBLIC_STRUCTMSG_H__

#include "../util/util.h"


#define SCMD_HEART 100	// ����Э��

#pragma pack (1)

struct MSG_BASE
{
	unsigned char		flags; // ��Ϣ��ʼ��־
	unsigned short int	size;  // ��Ϣ�ܳ���
	unsigned char		cmd1;  // һ����Ϣ��
	unsigned char		cmd2;  // ������Ϣ��
	unsigned short int  gameid;// ��Ϸid
	unsigned char		serverNo;// ���������
	unsigned int		uid;	// uid
	unsigned short int  arg1;
	unsigned char		arg2;
	unsigned int		arg3;
	unsigned char		flage; // ��Ϣ������־

	MSG_BASE()
	{
		memset(this, 0, sizeof(MSG_BASE));
	}
};

struct FDINFO
{
	uint16_t id;
	uint16_t fd;
};

union Session
{
	FDINFO data;
	uint32_t session;
}; 


#pragma  pack()

#endif