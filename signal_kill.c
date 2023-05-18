#include "signal_kill.h"

int signalKill(int argc, char *argv[])
{
	int ret = 0;
	if (argc < 3) {
		printf("error argument\n");	
		return -1;
	}

	int nsig = atoi(argv[1]+1);
	int pid = atoi(argv[2]);

	// kill(int pid,int sid):  
	// pid:接收信号的进程号，sid:发送的信号值
	// 当sid=0时，不发送信号值
	// 因此，源代码中需要+1
	// 成功执行时返回0，失败返回-1
	ret = kill(pid, nsig);
	if (ret) {
		printf("kill failed.\n");	
		perror("kill");
	} else {
		printf("%d killed.\n", pid);
	}

	return ret;
}

