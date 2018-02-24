#ifndef __PUBLIC_STRUCTMSG_H__
#define __PUBLIC_STRUCTMSG_H__

#include "../util/util.h"


#define SCMD_HEART 100	// 心跳协议

#pragma pack (1)

struct MSG_BASE
{
	unsigned char		flags; // 消息起始标志
	unsigned short int	size;  // 消息总长度
	unsigned char		cmd1;  // 一级消息号
	unsigned char		cmd2;  // 二级消息号
	unsigned short int  gameid;// 游戏id
	unsigned char		serverNo;// 服务器编号
	unsigned int		uid;	// uid
	unsigned short int  arg1;
	unsigned char		arg2;
	unsigned int		arg3;
	unsigned char		flage; // 消息结束标志

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