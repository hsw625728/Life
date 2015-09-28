#ifndef __GMSV_PROTO_H__
#define __GMSV_PROTO_H__

#include "proto_info.h"

enum
{
	eACCOUNT_STATUS_CLIENTLOGIN_WAIT,
};
typedef struct{
	int clid;
	int idkey_len;
	int pass_len;
	char IDKey[IDKEY_SIZE];
	char pass[IDKEY_SIZE];
	char memberID[AUTH_MEMBERID_SIZE];
	char authString[AUTH_STRING_SIZE];
	unsigned char physicalAddress[8];
}sSYSTEM_INFO_CLIENT_ACCOUNT;


#endif
