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

#include "net_packet.h"
#include "table_header.h"
#include "protocol.h"
#include "proto_info.h"
#include "log.h"
#include "init.h"
#include "resource.h"
#include "db.h"
#include "all_player.h"
#include "util.h"
#include "net.h"
#include "dbsvproto_cli.h"
#include "dbsv_proto.h"
#include "dbsvProtoList.h"
#include "gmsvproto_sv.h"
#include "gmsv_proto.h"
#include "account/account.h"
#include "timer_life.h"

enum
{
	eCTYPE_BOOL,
	eCTYPE_INT,
	eCTYPE_STRING
}CTYPE;

#endif
