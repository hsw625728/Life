#ifndef __NET_PACKET_H__
#define __NET_PACKET_H__
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include "type.h"

#define DEFAULT_SIZE 0x1000
#define DEFAULT_INCREASE_SIZE 200

struct NetPacket
{
	uint16_t opcode;
	uint8_t* m_buffer;
	size_t m_readPos;
	size_t m_writePos;
	uint32_t m_buffersize;
};

struct sock_ev
{
	struct event* read_ev;
	struct event* write_ev;
	struct NetPacket* packet;
	int sockid;
};

void netpackReserve(struct NetPacket* packet, size_t bufferSize);
	
#define NET_PACKET_BEGIN(a,b) struct NetPacket* a = (struct NetPacket*)malloc(sizeof(struct NetPacket));netpackInit(a, b);
#define NET_PACKET_END(a) netpackRelease(a);

#define NET_PACKET_READ(a) a netpacketRead##a(struct NetPacket* packet)\
{\
	if (packet->m_readPos + sizeof(a) > packet->m_writePos)\
		return (a)0;\
	a ret = *(a*)&(packet->m_buffer[packet->m_readPos]);\
	packet->m_readPos += sizeof(a);\
	return ret;\
}
	
#define NET_PACKET_WRITE(a, packet) void netpacketWrite##a(struct NetPacket* packet, const a data)\
{\
	size_t new_size = packet->m_writePos + sizeof(a);\
	if (new_size > packet->m_writePos)\
	{\
		new_size = (new_size / DEFAULT_INCREASE_SIZE + 1) * DEFAULT_INCREASE_SIZE;\
		netpackReserve(packet, new_size);\
	}\
	*(a*)(&packet->m_buffer[packet->m_writePos]) = data;\
	packet->m_writePos += sizeof(a);\
}



#define NET_PACKET_POP_UINT64(packet, re) re = netpacketReaduint64_t(packet)
#define NET_PACKET_POP_UINT32(packet, re) re = netpacketReaduint32_t(packet)
#define NET_PACKET_POP_UINT16(packet, re) re = netpacketReaduint16_t(packet)
#define NET_PACKET_POP_UINT8(packet, re) re = netpacketReaduint8_t(packet)
#define NET_PACKET_POP_INT64(packet, re) re = netpacketReadint64_t(packet)
#define NET_PACKET_POP_INT32(packet, re) re = netpacketReadint32_t(packet)
#define NET_PACKET_POP_INT16(packet, re) re = netpacketReadint16_t(packet)
#define NET_PACKET_POP_INT8(packet, re) re = netpacketReadint8_t(packet)
#define NET_PACKET_POP_FLOAT(packet, re) re = netpacketReadfloat(packet)
#define NET_PACKET_POP_DOUBLE(packet, re) re = netpacketReaddouble(packet)
#define NET_PACKET_POP_STRING(a,b,c) netpackReadString(a, b, c)
#define NET_PACKET_POP_BUFFER(a,b,c) netpackReadbuff(a, b, c)



#define NET_PACKET_PUSH_UINT64(packet, data) netpacketWriteuint64_t(packet, data)
#define NET_PACKET_PUSH_UINT32(packet, data) netpacketWriteuint32_t(packet, data)
#define NET_PACKET_PUSH_UINT16(packet, data) netpacketWriteuint16_t(packet, data)
#define NET_PACKET_PUSH_UINT8(packet, data) netpacketWriteuint8_t(packet, data)
#define NET_PACKET_PUSH_INT64(packet, data) netpacketWriteint64_t(packet, data)
#define NET_PACKET_PUSH_INT32(packet, data) netpacketWriteint32_t(packet, data)
#define NET_PACKET_PUSH_INT16(packet, data) netpacketWriteint16_t(packet, data)
#define NET_PACKET_PUSH_INT8(packet, data) netpacketWriteint8_t(packet, data)
#define NET_PACKET_PUSH_FLOAT(packet, data) netpacketWritefloat(packet, data)
#define NET_PACKET_PUSH_DOUBLE(packet, data) netpacketWritedouble(packet, data)
#define NET_PACKET_PUSH_STRING(a,b) netpackWriteString(a,b)
#define NET_PACKET_PUSH_BUFFER(a,b) netpackWriteBuff(a,b)
#define NET_PACKET_UPDATE_LENGTH(packet) netpacketUpdatePacketLength(packet)


void netpackSetOpcode(struct NetPacket* packet, unsigned short opcode);

void netpackInit(struct NetPacket* packet, size_t bufferSize);

void netpackReserve(struct NetPacket* packet, size_t bufferSize);

unsigned int netpackGetBufferSize(const struct NetPacket* const packet);

void netpackReadbuff(struct NetPacket* packet, unsigned char* buffer, size_t len);

void netpackWriteBuff(struct NetPacket* packet, const unsigned char* buffer, size_t len);

BOOL netpackReadString(struct NetPacket* packet, unsigned char* buff, size_t bufflen);

BOOL netpackWriteString(struct NetPacket* packet, const unsigned char* str);

void netpackRelease(struct NetPacket* packet);

void netpackGetOpcode(struct NetPacket* packet, unsigned short* op);
void netpacketUpdatePacketLength(struct NetPacket* packet);
#endif
