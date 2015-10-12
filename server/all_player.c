#include "system.h"
#include <signal.h>

AllPlayer* allPlayer = 0;
int allPlayerNum = 0;

PlayerSchedule* playerSchedule = 0;

BOOL initAllPlayer(void)
{
	db_readAllPlayer();
}

void updatePlayerSockid(const char* id, int sockid)
{
	int i = 0;
	for (; i < allPlayerNum; i++)
	{
		if (!strcmp(allPlayer[i].ID, id))
		{
			allPlayer[i].sockid = sockid;
			printf("UpdatePlayerSockid(%s)=%d\n", id, sockid);
			break;
		}
	}
}
void updatePlayerSchedule(const char* id)
{
	db_readPlayerSchedule(id);
	printf("UpdatePlayerSchedule.id=%s\n", id);
}
void updateAllPlayer(int signo)
{
	if (signo == SIGALRM)
	{
		int i = 0;
		printf ("Receive SIGALRM\n");
		for (; i < allPlayerNum; i++)
		{
			if (allPlayer[i].sockid != 0)
			{
				int act;
				unsigned int elapse;
				PlayerSchedule* ps = getPlayerScheduleByid(allPlayer[i].ID);
				getCurrentActionAndElapse(ps, &act, &elapse);
				//这里给在线的角色发送数据
				char string[256];
				sprintf(string, "当前事件ID（%d）持续时间（%d）\n", act, elapse);
				send(allPlayer[i].sockid, string, strlen(string), 0);
			}
		}
		signal(SIGALRM, updateAllPlayer);
	}
}
PlayerSchedule* getPlayerScheduleByid(const char* id)
{
	int i = 0;
	for (; i < allPlayerNum; i++)
	{
		if (!strcmp(playerSchedule[i].ID, id))
		{
			return &(playerSchedule[i]);
		}
	}

	return NULL;
}
unsigned int calElapseFromTwoTimestr(const char* t1, const char* t2)
{
	time_t ti1,ti2;
	struct tm time1;
	struct tm time2;
	strptime(t1, "%Y-%m-%d %H:%M:%S", &time1);
	strptime(t2, "%Y-%m-%d %H:%M:%S", &time2);
	ti1 = mktime(&time1);
	ti2 = mktime(&time2);

	return (ti2 - ti1)/60;
}
void getCurrentActionAndElapse(PlayerSchedule* ps, int* act, unsigned int* elapse)
{
	int i;
	char fmttime[64];
	struct tm* strtime;
	time_t now;
	time(&now);
	strtime = localtime(&now);
	strftime(fmttime, sizeof(fmttime), "%Y-%m-%d %H:%M:%S", strtime);

	for (i = 0; i < ps->act_num; i++)
	{
		int res = strcmp(fmttime, ps->act_time[i]);
		if (res >= 0)
		{
			//记录当前正在进行的事件
			*act = ps->actid[i];

			//计算当前事件已经持续的时间
			*elapse = calElapseFromTwoTimestr(ps->act_time[i], fmttime);
		}
	}
}


















