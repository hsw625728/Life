#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

enum{
	MSG_CS_LOGIN = 1,
	MSG_SC_LOGIN_RES,
	MSG_CS_CREATE_NICKNAME,
	MSG_SC_CREATE_NICKNAME_RES,
	MSG_CS_PULL_STATE,
	MSG_SC_PULL_STATE_RES,
	MSG_CS_GET_SCHEDULE,
	MSG_SC_GET_SCHEDULE_RES,
	MSG_SC_UPDATE_PLAYER_STATE,
	MSG_CS_DOMINO_UPLOAD_TILEMAP = 100,
	MSG_SC_DOMINO_UPLOAD_TILEMAP_RES,
	MSG_CS_DOMINO_DOWNLOAD_TILEMAP,
	MSG_SC_DOMINO_DOWNLOAD_TILEMAP_RES,
};






#endif
