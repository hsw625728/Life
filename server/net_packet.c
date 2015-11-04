#include "net_packet.h"


NET_PACKET_READ(uint64_t)
NET_PACKET_READ(uint32_t)
NET_PACKET_READ(uint16_t)
NET_PACKET_READ(uint8_t)
NET_PACKET_READ(int64_t)
NET_PACKET_READ(int32_t)
NET_PACKET_READ(int16_t)
NET_PACKET_READ(int8_t)
NET_PACKET_READ(float)
NET_PACKET_READ(double)


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


void netpackSetOpcode(struct NetPacket* packet, unsigned short opcode)
{
	packet->opcode = opcode;
}

void netpackInit(struct NetPacket* packet, size_t bufferSize)
{
	packet->opcode = 0;
	packet->m_buffer = 0;
	packet->m_readPos = 0;
	packet->m_writePos = 0;
	packet->m_buffersize = 0;
	netpackReserve(packet, bufferSize);
}

void netpackReserve(struct NetPacket* packet, size_t bufferSize)
{
	if (packet->m_buffer)
		packet->m_buffer = (unsigned char*)realloc(packet->m_buffer, bufferSize);
	else
		packet->m_buffer = (unsigned char*)malloc(bufferSize);

	packet->m_buffersize = bufferSize;
}

unsigned int netpackGetBufferSize(const struct NetPacket* const packet)
{
	return packet->m_writePos;
}

void netpackReadbuff(struct NetPacket* packet, unsigned char* buffer, size_t len)
{
	if (packet->m_readPos + len > packet->m_writePos)
		len = (packet->m_writePos - packet->m_readPos);
	memcpy(buffer, &(packet->m_buffer[packet->m_readPos]), len);
	packet->m_readPos += len;
}

void netpackWriteBuff(struct NetPacket* packet, const unsigned char* buffer, size_t len)
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

BOOL netpackReadString(struct NetPacket* packet, unsigned char* buff, size_t bufflen)
{
	uint8_t len;
	len = netpacketReaduint8_t(packet);
	if (len == 0)
		return TRUE;
	else if(len > bufflen)
		return FALSE;
	unsigned int bufSize = netpackGetBufferSize(packet);
	if (bufSize < len)
		return FALSE;
	netpackReadbuff(packet, buff, len);
	buff[len] = 0;
	return TRUE;
}

BOOL netpackWriteString(struct NetPacket* packet, const unsigned char* str) 
{
	uint8_t len = strlen(str);
	netpacketWriteuint8_t(packet, len);
	if (len > 0)
		netpackWriteBuff(packet, str, len);	
}

void netpackRelease(struct NetPacket* packet)
{
	if (packet->m_buffer)
		free(packet->m_buffer);
}

void netpackGetOpcode(struct NetPacket* packet, unsigned short* op)
{
	NET_PACKET_POP_UINT16(packet, *op);
	//取得协议包的长度信息
	uint16_t length;
	NET_PACKET_POP_UINT16(packet, length);
	packet->m_writePos = length;
}

void netpacketUpdatePacketLength(struct NetPacket* packet)
{
	*(uint16_t*)&(packet->m_buffer[sizeof(packet->opcode)]) = packet->m_writePos;
}
