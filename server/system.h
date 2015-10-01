#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <event.h>

#include "table_header.h"
#include "protocol.h"
#include "proto_info.h"
#include "log.h"
#include "init.h"
#include "resource.h"
#include "net.h"
#include "db.h"
#include "util.h"
#include "dbsvproto_cli.h"
#include "dbsv_proto.h"
#include "dbsvProtoList.h"
#include "gmsvproto_sv.h"
#include "gmsv_proto.h"
#include "account/account.h"

enum
{
	eCTYPE_BOOL,
	eCTYPE_INT,
	eCTYPE_STRING
}CTYPE;

struct sock_ev {
	struct event* read_ev;
	struct event* write_ev;
	char* buffer;
	int sockid;
};

#endif

