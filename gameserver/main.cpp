#include <iostream>
#include <string>
#include "../public/config/config.h"
#include "../public/log/log.h"
#include "../public/timer/timermgr.h"

static void a_timer(timer* tmr, void* param)
{
	  printf("%s timer a \n",
           __FUNCTION__);
}

int main(int argc, char* argv[])
{
	ConfigMgr.parseConfig(argv[1]);
	ConfigMgr.printConfig();
	std::string str;
	ConfigMgr.getConfigInfo("FILE_SIZE", str);
	std::cout<<"ConfigMgr filesize:" << str <<std::endl;

	CLogMgr.init_log();
	TimerMgr.initTimer();
	TimerMgr.print_timer_info();
	TimerMgr.addTimer(1000, 500, 0, a_timer, NULL);
	TimerMgr.addTimer(2000, 500, 500, a_timer, NULL);
	static int index = 0;
	while (true)
	{
		//INFO("index:%d", index++);
		sleep(10);
		if (index++ > 20)
		{
			printf("index:%d\n", index);
			TimerMgr.delete_all_timer();
		}
	}

	return 0;
}