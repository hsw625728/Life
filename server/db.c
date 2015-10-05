#include "system.h"
#include <mysql/mysql.h>

MYSQL mysql;
MYSQL* gpMysql;

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


















