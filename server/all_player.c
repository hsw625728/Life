#include "system.h"
#include <signal.h>

AllPlayer* allPlayer = 0;
int allPlayerNum = 0;

PlayerSchedule* playerSchedule = 0;

//设定多久间隔给客户端发送一次状态（单位：秒）
const unsigned short UPDATE_ALLPLAYER_TIME = 5;
//一天中包含多少个tick_day
#define NUM_OF_TICK_DAY 24*60*60/UPDATE_ALLPLAYER_TIME
//计算何时进入新的一天
static unsigned int g_tick_day = 0;

BOOL initAllPlayer(void)
{
	db_readAllPlayer();
	//计算服务器启动当前时刻已经经过多少个g_tick_day
	time_t ti;
	struct tm* t;
	ti = time(NULL);
	t = localtime(&ti);
	g_tick_day = (t->tm_hour * 60 * 60 + t->tm_min * 60 + t->tm_sec)/UPDATE_ALLPLAYER_TIME;
}

void updatePlayerSockid(const char* id, int sockid)
{
	int i = 0;
	for (; i < allPlayerNum; i++)
	{
		if (!strcmp(allPlayer[i].usr, id))
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
void createNewDayScheduleForAllPlayer(void)
{
	//这里更新每天的日程表
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

		//计算新的一天是否开始
		g_tick_day++;
		if (g_tick_day > NUM_OF_TICK_DAY)
		{
			char string[256];
			g_tick_day = 0;
			createNewDayScheduleForAllPlayer();
			sprintf(string, "[SCHEDULE]Create a new day schedule for all player!\n");
			LogWrite(LT_SYSTEM, string);
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


















