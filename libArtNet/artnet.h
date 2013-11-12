#pragma once

#include <stdint.h>
#include <WinSock2.h>

#define ARTNET_MAX_PACKET_SIZE 1024
typedef struct SArtNetHeader
{
	uint8_t  Magic[8];
	uint16_t Opcode;   //Little Endian
	uint16_t ProtocolVersion;	
} ArtNetHeader;

typedef struct SArtNetDatagram
{
	ArtNetHeader* Header;
	uint8_t*	  PacketHeader;
	uint8_t*      Payload;
} ArtNetDatagram;

typedef BOOL (*ArtNetPacketDecodeFunc)(ArtNetDatagram* Datagram, char* Payload, size_t PayloadLength);

typedef BOOL (*ArtNetPacketEncodeFunc)(ArtNetDatagram* Datagram, uint8_t *Buffer, size_t *Length);

BOOL ArtNetReceive(SOCKET Socket, ArtNetDatagram* Datagram, struct sockaddr *From, int* FromLength);

BOOL ArtNetEncode(ArtNetDatagram* Datagram, uint8_t *Buffer, size_t *Length);