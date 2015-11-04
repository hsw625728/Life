#include "system.h"
#include <mysql/mysql.h>
#include "all_player.h"

MYSQL mysql;
MYSQL* gpMysql;
typedef struct
{
	char ID[33];
	int aID;
	float total_time;
	char update_time[64];
}PlayerHistory;

extern AllPlayer* allPlayer;
extern int allPlayerNum;
extern PlayerSchedule* playerSchedule;

BOOL initDB(void)
{
	char string[256];
	LogWrite(LT_SYSTEM, "Init: DB: Initialize MYSQL.\n");
	gpMysql = NULL;
	if (!mysql_init(&mysql))
	{
		sprintf(string, "MYSQL initialize Error.");
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}
	gpMysql = mysql_real_connect(&mysql, ConfigData.mysqlHostName, ConfigData.mysqlUserName, ConfigData.mysqlPassword, ConfigData.mysqlDatabase, 0, NULL, 0);
	printf("Connect %s,%s,%s,%s\n", ConfigData.mysqlHostName, ConfigData.mysqlUserName, ConfigData.mysqlPassword, ConfigData.mysqlDatabase);
	if (!gpMysql)
	{
		sprintf(string, "Mysql Connect Error (%s).\n", mysql_error(&mysql));
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}
	if (!mysql_set_character_set(&mysql, "utf8"))
	{
		sprintf(string, "Mysql set character to (%s).\n", mysql_character_set_name(&mysql));
		LogWrite(LT_SYSTEM, string);
	}
	sprintf(string, "Init: DB: DB connected successfully!\n");
	printf(string);
	LogWrite(LT_SYSTEM, string);
	return TRUE;
}

BOOL sendQuery(char* fmt, ...)
{
	va_list ap;

	int result;
	char query[LENGTH_OF_QUERY];
	char escaped[LENGTH_OF_QUERY];
	char string[256];
	char *p;
	char *sval;
	char *sql;

	sql = query;
	va_start(ap, fmt);
	for (p = fmt; *p; ++p)
	{
		if (*p != '%')
		{
			*(sql++) = *p;
			continue;
		}
		switch(*(++p))
		{
			case 'd':
				*sql = '\0';
				sprintf(escaped, "%d", va_arg(ap, int));
				strcat(sql, escaped);
				sql += strlen(escaped);
				break;
			case 'S':
			case 'c':
				sval = va_arg(ap, char*);
				sql += sprintf(sql, "%s", sval);
				break;
			case 's':
				sval = va_arg(ap, char*);
				mysql_real_escape_string(gpMysql, escaped, sval, strlen(sval));
				sql += sprintf(sql, "'%s'", escaped);
				break;
			default:
				*sql++ = *p;
				break;
		}
	}

	va_end(ap);
	*sql = '\0';

	result = mysql_real_query(gpMysql, query, strlen(query));

	if (result != 0)
	{
		sprintf(string, "Mysql query error (%s).\n", mysql_error(gpMysql));
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}
	printf(query);
	printf("\n");
	return TRUE;
}

BOOL closeDB(void)
{
	LogWrite(LT_SYSTEM, "DB: Close MYSQL Begin.\n");
	mysql_close(gpMysql);
	LogWrite(LT_SYSTEM, "DB: Close MYSQL End.\n");
	return TRUE;
}

int readUsrAndPwd(void)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	int i;
	int rec;
	char string[256];

	BOOL cc;

	cc = sendQuery("select * from accountinfo");
	if (!cc)
	{
		return 0;
	}

	res = mysql_store_result(gpMysql);

	if (!res)
	{
		sprintf(string, "Mysql read usrAndPwd error!\n");
		LogWrite(LT_SYSTEM, string);
		return 0;
	}

	rec = mysql_num_rows(res);
	for (i = 0; i < rec; ++i)
	{
		row = mysql_fetch_row(res);
		printf(row[eUSER_NAME]);
		printf("\n");
		printf(row[ePASS_WORD]);
		printf("\n");
	}

	mysql_free_result(res);
	return rec;
}

BOOL db_readAllPlayer(void)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	int i,rec;
	BOOL cc;
	char string[256];
	cc = sendQuery("select * from account_info");
	if (!cc) return FALSE;
	res = mysql_store_result(gpMysql);
	if (!res)
	{
		sprintf(string, "Mysql read allplayer error!\n");
		LogWrite(LT_SYSTEM,string);
		return FALSE;
	}	
	rec = mysql_num_rows(res);
	allPlayer = (AllPlayer*)malloc(rec * sizeof(AllPlayer));
	playerSchedule = (PlayerSchedule*)malloc(rec * sizeof(PlayerSchedule));
	allPlayerNum = rec;
	for (i = 0; i < rec; ++i)
	{
		row = mysql_fetch_row(res);
		AllPlayer* player = &(allPlayer[i]);
		strcpy(player->ID, row[eACCOUNT_INFO_ID]);
		strcpy(player->usr, row[eACCOUNT_INFO_USR]);
		strcpy(player->nickname, row[eACCOUNT_INFO_NICKNAME]);
		strcpy(player->birthday, row[eACCOUNT_INFO_BIRTHDAY]);
		strcpy(player->pwd, row[eACCOUNT_INFO_PWD]);
		player->sockid = 0;

		PlayerSchedule* ps = &(playerSchedule[i]);
		strcpy(ps->ID, row[eACCOUNT_INFO_ID]);
		ps->actid = NULL;
		ps->act_num = 0;

		printf(player->ID);
		printf("\n");
		printf(player->usr);
		printf("\n");
		printf(player->nickname);
		printf("\n");
		printf(player->birthday);
		printf("\n");
		printf(player->pwd);
		printf("\n");
	}
	mysql_free_result(res);
	return TRUE;
}

int db_readPlayerSchedule(const char* id)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	int i, rec;
	BOOL cc;
	char string[256];
	cc = sendQuery("select * from schedule_daily where ID=%s", id);
	if (!cc) return FALSE;
	res = mysql_store_result(gpMysql);
	if (!res)
	{
		sprintf(string, "Mysql read allplayer error!\n");
		LogWrite(LT_SYSTEM,string);
		return FALSE;
	}	
	rec = mysql_num_rows(res);
	if (rec > 0)
	{
		PlayerSchedule* ps = getPlayerScheduleByid(id);
		ps->act_num = rec;
		ps->actid = (int*)malloc(rec * sizeof(int));
		ps->act_time = (char**)malloc(rec * sizeof(char*));
		for (i = 0; i < rec; i++)
		{
			row = mysql_fetch_row(res);
			ps->actid[i] = atoi(row[eSCHEDULE_DAILY_AID]);
			ps->act_time[i] = (char*)malloc(64 * sizeof(char));
			strcpy(ps->act_time[i], row[eSCHEDULE_DAILY_TIME]);
			printf("ps->actid[%d]=%d\n", i, ps->actid[i]);
			printf("ps->act_time[%d]=%s\n", i, ps->act_time[i]);
		}
	}
	mysql_free_result(res);
	return rec;
}



int db_updatePlayerHistory(const char* id)
{
	int aID;
	char calTimeStr[64];
	time_t updateTime;
	struct tm updateTimeTM;
	char updateTimeStr[64];
	char updateTimeStr_T[64];
	time_t now;
	struct tm* now_tm;
	char now_str[64];
	char now_str_T[64];
	PlayerHistory* phistory = 0;
	int history_num = 0;
	PlayerHistory* update_his = 0;
	int update_his_num = 0;
	PlayerSchedule* schedule = 0;

	time(&now);
	now_tm = localtime(&now);
	strftime(now_str, sizeof(now_str), "%Y-%m-%d %H:%M:%S", now_tm);
	strftime(now_str_T, sizeof(now_str_T), "%H:%M:%S", now_tm);


	MYSQL_RES* res;
	MYSQL_ROW row;
	int i, rec;
	BOOL cc;
	char string[256];

	//第一步：从数据库中把指定id的角色历史时间读取到内存中
	cc = sendQuery("SELECT * FROM player_history WHERE player_history.ID=%s", id);
	if (!cc) return FALSE;
	res =mysql_store_result(gpMysql);
	if (!res)
	{
		sprintf(string, "[SQL_ERROR]db.c:db_updatePlayerHistory() sql err!\n");
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}
	rec = mysql_num_rows(res);
	if (rec > 0)
	{
		phistory = (PlayerHistory*)malloc(sizeof(PlayerHistory) * rec);
		history_num = rec;
		for (i = 0; i < rec; i++)
		{
			row = mysql_fetch_row(res);
			strcpy(phistory[i].ID, row[ePLAYER_HISTORY_ID]);
			phistory[i].aID = atoi(row[ePLAYER_HISTORY_AID]);
			phistory[i].total_time = atof(row[ePLAYER_HISTORY_TOTAL_TIME]);
			strcpy(phistory[i].update_time, row[ePLAYER_HISTORY_UPDATE_TIME]);
		}
		memset(updateTimeStr, 0, 64);
		strcpy(updateTimeStr, row[ePLAYER_HISTORY_UPDATE_TIME]); 
	}
	else
	{
		AllPlayer* player = getPlayerByID(id);
		if (player == 0)
		{
			sprintf(string, "[Player]db.c:db_updatePlayerHistory().get playerid=(%s)failed!\n", id);
			LogWrite(LT_SYSTEM, string);
			return -1;
		}
		strcpy(updateTimeStr, player->birthday);
	}
	
	strcpy(calTimeStr, updateTimeStr);
	strptime(updateTimeStr, "%Y-%m-%d %H:%M:%S", &updateTimeTM);
	updateTime = mktime(&updateTimeTM);
	strftime(updateTimeStr_T, sizeof(updateTimeStr_T), "%H:%M:%S", &updateTimeTM);
	printf("now_str_T=(%s),updateTimeStr_T=(%s)\n", now_str_T, updateTimeStr_T);

	mysql_free_result(res);

	//第二步：计算并更新内存数据中角色的最新历史时间
	int elapse = now - updateTime;
	int day = elapse/(60*60*24);
	int min = elapse%(60*60*24);
	schedule = getPlayerScheduleByid(id);
	if (schedule != 0)
	{
		update_his_num = schedule->act_num + history_num;
		update_his = (PlayerHistory*)malloc(sizeof(PlayerHistory)*update_his_num);
		memset(update_his, 0, sizeof(PlayerHistory)*update_his_num);
		memcpy(update_his, phistory, sizeof(PlayerHistory)*history_num);
		//累计整天的部分
		if (day > 0)
		{
			for (i = 0; i < schedule->act_num; i++)
			{
				float ti = 0.0f;
				if (i < schedule->act_num - 1)
					ti = calElapseFromTwoTimeString(schedule->act_time[i], schedule->act_time[i+1]);
				else
				{
					float temp1 = 0.0f;
					float temp2 = 0.0f;
					temp1 = calElapseFromTwoTimeString(schedule->act_time[i], "23:59:59");
					temp2 = calElapseFromTwoTimeString("00:00:00", schedule->act_time[0]);
					ti = temp1 + temp2;
					ti += 1.0f;
				}

				int j;
				for (j = 0; j < history_num; j++)
				{
					//历史记录已经存在。直接累积时间。
					if (schedule->actid[i] == update_his[j].aID)
					{
						update_his[j].total_time += (ti*day);
						strcpy(update_his[j].update_time, now_str);
						break;
					}
				}
				if (j == history_num)
				{
					//历史记录中不存在，添加到历史记录中。
					strcpy(update_his[history_num].ID, id);
					update_his[history_num].aID = schedule->actid[i];
					update_his[history_num].total_time += (ti*day);
					strcpy(update_his[history_num].update_time, now_str);

					history_num++;
				}
			}	
		}
		//累计不足整天的部分
		int haf = strcmp(updateTimeStr_T, now_str_T); 
		if (haf >= 0)
		//if(0)
		{
			float up_i = 0.0f;
			int aID = 0;
			//不足一天计算两端
			for (i = 0; i < schedule->act_num; i++)
			{
				int big = strcmp(now_str_T, schedule->act_time[i]);
				int small = strcmp(schedule->act_time[i], updateTimeStr_T);
				if (big >=0)
				{
					float ti = 0.0f;
					if (i < schedule->act_num - 1)
					{
						if (strcmp(now_str_T, schedule->act_time[i+1]) > 0)
						{
							ti = calElapseFromTwoTimeString(schedule->act_time[i], schedule->act_time[i+1]);
						}
						else
						{
							ti = calElapseFromTwoTimeString(schedule->act_time[i], now_str_T);
						}
					}
					else
					{
						ti = calElapseFromTwoTimeString(schedule->act_time[i], now_str_T);
					}
					int j;
					for (j = 0; j < history_num; j++)
					{
						if (schedule->actid[i] == update_his[j].aID)
						{
							update_his[j].total_time += ti;	
							strcpy(update_his[j].update_time, now_str);
							break;
						}
					}
					if (j == history_num)
					{
						//历史记录中不存在，添加到历史记录中。
						strcpy(update_his[history_num].ID, id);
						update_his[history_num].aID = schedule->actid[i];
						update_his[history_num].total_time += ti;
						strcpy(update_his[history_num].update_time, now_str);
	
						history_num++;
						printf("1---->%d\n", history_num);
					}
					strcpy(calTimeStr, now_str);
				}
				else if (small >= 0)
				{
					float ti = 0.0f;
					if (i > 0)
					{
						if (strcmp(schedule->act_time[i], updateTimeStr_T)>0&&strcmp(schedule->act_time[i-1], updateTimeStr_T)<0)
						{
							up_i = calElapseFromTwoTimeString(updateTimeStr_T, schedule->act_time[i]);
							aID = schedule->actid[i-1];
						}
					}
					else
					{
						
						up_i = calElapseFromTwoTimeString(updateTimeStr_T, schedule->act_time[i]);
						aID = schedule->actid[history_num - 1];
					}
					
					if (i < schedule->act_num - 1)
					{
						ti = calElapseFromTwoTimeString(updateTimeStr_T, schedule->act_time[i+1]);
					}
					else
					{
						ti = calElapseFromTwoTimeString(schedule->act_time[i], "23:59:59");
					}
					int j;
					for (j = 0; j < history_num; j++)
					{
						if (schedule->actid[i] == update_his[j].aID)
						{
							update_his[j].total_time += ti;
							strcpy(update_his[j].update_time, now_str);
							break;
						}
					}
					if (j == history_num)
					{
						//历史记录中不存在，添加到历史记录中。
						strcpy(update_his[history_num].ID, id);
						update_his[history_num].aID = schedule->actid[i];
						update_his[history_num].total_time += ti;
						strcpy(update_his[history_num].update_time, now_str);
	
						history_num++;
						printf("2---->%d\n", history_num);
					}
					strcpy(calTimeStr, now_str);

				}
			}

			//计算update下面一段和now_str上面一段
			int j;
			for (j = 0; j < history_num; j++)
			{
				if (aID == update_his[j].aID)
				{
					update_his[j].total_time += up_i;
					strcpy(update_his[j].update_time, now_str);
					break;
				}
			}
			
			float down_now = 0.0f;
			
			if (strcmp(schedule->act_time[0], now_str_T)>0)
			{
				down_now = calElapseFromTwoTimeString("00:00:00", now_str_T);
				aID = schedule->actid[history_num - 1];
			}
			else
			{
				down_now = calElapseFromTwoTimeString("00:00:00", schedule->act_time[0]);
				aID = schedule->actid[history_num - 1];
			}
			for (j = 0; j < history_num; j++)
			{
				if (aID == update_his[j].aID)
				{
					update_his[j].total_time += down_now;
					strcpy(update_his[j].update_time, now_str);
					break;
				}
			}
	
		}
		else
		{
			//超过一天计算中间
			for (i = 0; i < schedule->act_num; i++)
			{
				int big = strcmp(now_str_T, schedule->act_time[i]);
				int small = strcmp(schedule->act_time[i], updateTimeStr_T);
				if (big >= 0 && small >= 0)
				{	
					float ti = 0.0f;
					//计算update到第一个事件的剩余时间累计到上次事件中
					if (i == 0)
					{
						ti = calElapseFromTwoTimeString(updateTimeStr_T, schedule->act_time[i]);
						aID = schedule->actid[schedule->act_num - 1];
						int j;
						for (j = 0; j < history_num; j++)
						{
							if (aID == update_his[j].aID)
							{
								update_his[j].total_time += ti;
								strcpy(update_his[j].update_time, now_str);
								break;
							}
						}
						if (j == history_num)
						{
							//历史记录中不存在，添加到历史记录中。
							strcpy(update_his[history_num].ID, id);
							update_his[history_num].aID = schedule->actid[i];
							update_his[history_num].total_time += ti;
							strcpy(update_his[history_num].update_time, now_str);
	
							history_num++;
						printf("3---->%d\n", history_num);
						}
					}
					else
					{
						if (strcmp(schedule->act_time[i-1], updateTimeStr_T)<0)
						{
							ti = calElapseFromTwoTimeString(updateTimeStr_T, schedule->act_time[i]);
							aID = schedule->actid[i-1];

							int j;
							for (j = 0; j < history_num; j++)
							{
								if (aID == update_his[j].aID)
								{
									update_his[j].total_time += ti;
									strcpy(update_his[j].update_time, now_str);
									break;
								}
							}
							if (j == history_num)
							{
								//历史记录中不存在，添加到历史记录中。
								strcpy(update_his[history_num].ID, id);
								update_his[history_num].aID = schedule->actid[i];
								update_his[history_num].total_time += ti;
								strcpy(update_his[history_num].update_time, now_str);
	
								history_num++;
								
						printf("4---->%d\n", history_num);
							}
						}
					}

					//计算事件到下次事件的时间
					if (i >= schedule->act_num - 1)
					{
						float ti = 0.0f;
						ti = calElapseFromTwoTimeString(schedule->act_time[i], now_str_T);
						aID = schedule->actid[i];
						int j;
						for (j = 0; j < history_num; j++)
						{
							if (aID == update_his[j].aID)
							{
								update_his[j].total_time += ti;
								strcpy(update_his[j].update_time, now_str);
								break;
							}
						}
						if (j == history_num)
						{
							//历史记录中不存在，添加到历史记录中。
							strcpy(update_his[history_num].ID, id);
							update_his[history_num].aID = schedule->actid[i];
							update_his[history_num].total_time += ti;
							strcpy(update_his[history_num].update_time, now_str);

							history_num++;
						printf("5---->%d\n", history_num);
						}
					}
					else
					{
						float ti = 0.0f;
						if (strcmp(schedule->act_time[i+1], now_str_T)<0)
						{
							ti = calElapseFromTwoTimeString(schedule->act_time[i], schedule->act_time[i+1]);
							aID = schedule->actid[i];
						}
						else
						{
							ti = calElapseFromTwoTimeString(schedule->act_time[i], now_str_T);
							aID = schedule->actid[i];
						}
						int j;
						for (j = 0; j < history_num; j++)
						{
							if (aID == update_his[j].aID)
							{
								update_his[j].total_time += ti;
								strcpy(update_his[j].update_time, now_str);
								break;
							}
						}
						if (j == history_num)
						{
							//历史记录中不存在，添加到历史记录中。
							strcpy(update_his[history_num].ID, id);
							update_his[history_num].aID = schedule->actid[i];
							update_his[history_num].total_time += ti;
							strcpy(update_his[history_num].update_time, now_str);

							history_num++;
						printf("6---->%d\n", history_num);
						}
					}

					strcpy(calTimeStr, now_str);
				}
			}
		}
	}

	//第三步：将内存中的数据更新写入数据库
	for (i = 0; i < history_num; i++)
	{
		char string[1024];
		memset(string, 0, 1024);
		sprintf(string , "REPLACE INTO player_history VALUES('%s',%d,%f,'%s')", update_his[i].ID, update_his[i].aID, update_his[i].total_time, calTimeStr);
		sendQuery(string);
		printf(string);
		printf("\n");
	}


	//第四步：释放临时申请内存空间
	if (phistory != 0)
		free(phistory);
	if (update_his != 0)
		free(update_his);
}

int db_updateDominoTilemap(char* usr, int index, const unsigned char* tilemap)
{
	char string[66*1024];
	memset(string, 0, 66*1024);
	sprintf(string, "REPLACE INTO domino_tilemap VALUES('%s',%d,'%s')", usr, index, tilemap);
	BOOL ret = sendQuery(string);
	if (ret)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int db_readDominoTilemap(char* usr, int index, unsigned char* tilemap)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	BOOL cc;
	char string[256];
	memset(string, 0, 256);
	cc = sendQuery("SELECT * FROM domino_tilemap WHERE ID=%s AND tileid=%d", usr, index);
	if (!cc)
		return -1;
	res = mysql_store_result(gpMysql);
	if (!res)
	{
		sprintf(string, "Mysql read DominoTilemap error!\n");
		LogWrite(LT_SYSTEM, string);
		return -2;
	}
	int rec = mysql_num_rows(res);
	if (rec > 0)
	{
		row = mysql_fetch_row(res);
		strcpy(tilemap, row[eDOMINO_TILEMAP_TILEMAP]);
		return 0;
	}
	return -3;
}














