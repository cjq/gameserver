#ifndef __PUBLIC_STRUCTMSG_STRUCTMSG_H__
#define __PUBLIC_STRUCTMSG_STRUCTMSG_H__

#include "../util/util.h"

struct TClientInfo
{
	int			id;
	int			port;
	char		hostname[50];
	uint32_t	sessionid;
	ZEROSTRUCT(TClientInfo);
};

struct TEventInfo
{
	void*	ptr1;
	void*	ptr2;
	int		fd;
	int		type;
	int		port;
	ZEROSTRUCT(TEventInfo);
};

struct TSocketInfo
{
	uint8_t type;  // 0 socket 1 timer
	ulong fd;
	uint32_t size; // data size
	TSocketInfo()
	{
		type = 0;
		fd = 0;
		size = sizeof(TSocketInfo);
	}
};

#endif