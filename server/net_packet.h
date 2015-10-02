#ifndef __NET_PACKET_H__
#define __NET_PACKET_H__
#include <stddef.h>
#include <inttypes.h>
#include "type.h"

#define DEFAULT_SIZE 0x1000
#define DEFAULT_INCREASE_SIZE 200

#define NET_PACKET_BEGIN(a,b) NetPacket a;netpackInit(a, b);
#define NET_PACKET_END(a) netpackRelease(a);

typedef struct 
{
	unsigned int opcode;
	unsigned char* m_buffer;
	size_t m_readPos;
	size_t m_writePos;
	unsigned int m_buffersize;
}NetPacket;

#define NET_PACKET_READ(a,packet) a netpacketRead##a(NetPacket* packet)\
{\
	if (packet->m_readPos + sizeof(a) > packet->m_writePos)\
		return (a)0;\
	a ret = *(a*)&(packet->m_buffer[packet->m_readPos]);\
	packet->m_readPos += sizeof(a);\
	return ret;\
}
	
#define NET_PACKET_WRITE(a, packet) void netpacketWrite##a(NetPacket* packet, const a data)\
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


NET_PACKET_READ(uint64_t, packet)
NET_PACKET_READ(uint32_t, packet)
NET_PACKET_READ(uint16_t, packet)
NET_PACKET_READ(uint8_t, packet)
NET_PACKET_READ(int64_t, packet)
NET_PACKET_READ(int32_t, packet)
NET_PACKET_READ(int16_t, packet)
NET_PACKET_READ(int8_t, packet)
NET_PACKET_READ(float, packet)
NET_PACKET_READ(double, packet)

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
#define NET_PACKET_POP_STRING(a,b) netpackReadString(a, b)


NET_PACKET_WRITE(uint64_t, packet)
NET_PACKET_WRITE(uint32_t, packet)
NET_PACKET_WRITE(uint16_t, packet)
NET_PACKET_WRITE(uint8_t, packet)
NET_PACKET_WRITE(int64_t, packet)
NET_PACKET_WRITE(int32_t, packet)
NET_PACKET_WRITE(int16_t, packet)
NET_PACKET_WRITE(int8_t, packet)
NET_PACKET_WRITE(float, packet)
NET_PACKET_WRITE(double, packet)

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


void netpackSetOpcode(NetPacket* packet, unsigned int opcode)
{
	packet->opcode = opcode;
}

void netpackInit(NetPacket* packet, size_t bufferSize)
{
	packet->opcode = 0;
	packet->m_buffer = 0;
	packet->m_readPos = 0;
	packet->m_writePos = 0;
	packet->m_buffersize = 0;
	netpackReserve(packet, bufferSize);
}

void netpackReserve(NetPacket* packet, size_t bufferSize)
{
	if (packet->m_buffer)
		packet->m_buffer = (unsigned char*)realloc(packet->m_buffer, bufferSize);
	else
		packet->m_buffer = (unsigned char*)malloc(bufferSize);

	packet->m_buffersize = bufferSize;
}

unsigned int netpackGetBufferSize(const NetPacket* const packet)
{
	return packet->m_writePos;
}

void netpackReadbuff(NetPacket* packet, unsigned char* buffer, size_t len)
{
	if (packet->m_readPos + len > packet->m_writePos)
		len = (packet->m_writePos - packet->m_readPos);
	memcpy(buffer, &(packet->m_buffer[packet->m_readPos]), len);
	packet->m_readPos += len;
}

void netpackWriteBuff(NetPacket* packet, const unsigned char* buffer, size_t len)
{
	size_t new_size = packet->m_writePos + len;
	if (new_size > packet->m_buffersize)
	{
		new_size = (new_size / DEFAULT_INCREASE_SIZE + 1) * DEFAULT_INCREASE_SIZE;
		netpackReserve(packet, new_size);
	}
	memcpy(&(packet->m_buffer[packet->m_writePos]), buffer, len);
	packet->m_writePos += len;
}

BOOL netpackReadString(NetPacket* packet, unsigned char** buff, size_t bufflen)
{
	uint32_t len;
	len = netpacketReaduint32_t(packet);
	if (len == 0)
		return TRUE;
	else if(len > bufflen)
		return FALSE;
	unsigned int bufSize = netpackGetBufferSize(packet);
	if (bufSize < len)
		return FALSE;
	netpackReadbuff(packet, *buff, len);
	(*buff)[len] = 0;
	return TRUE;
}

BOOL netpackWriteString(NetPacket* packet, const unsigned char* str) 
{
	uint32_t len = strlen(str);
	netpacketWriteuint8_t(packet, len);
	if (len > 0)
		netpackWriteBuff(packet, str, len);	
}

void netpackRelease(NetPacket* packet)
{
	if (packet->m_buffer)
		free(packet->m_buffer);
}

#endif
