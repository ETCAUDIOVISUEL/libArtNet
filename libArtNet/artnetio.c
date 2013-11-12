#include "stdafx.h"
#include <assert.h>

ArtNetPacketDecodeFunc INTPacketDecodeFunctions[0xFFFF];
ArtNetPacketEncodeFunc INTPacketEncodeFunctions[0xFFFF];

uint16_t INTFlipByteOrder( uint16_t val ) 
{
    return (val << 8) | (val >> 8 );
}

BOOL ArtNetReceive(SOCKET Socket, ArtNetDatagram* Datagram, struct sockaddr *From, int* FromLength)
{
	char *DatagramBuffer;
	int DatagramLength;
	assert(Datagram != 0);
	DatagramBuffer = (char *) malloc(ARTNET_MAX_PACKET_SIZE);
	DatagramLength = recvfrom(Socket, (char *) DatagramBuffer, ARTNET_MAX_PACKET_SIZE, 0, From, FromLength);
	if (DatagramLength < (int)sizeof(ArtNetHeader))
	{
		free(DatagramBuffer);
		return FALSE;
	}
	Datagram->Header = (ArtNetHeader *) DatagramBuffer;
#ifndef ARTNET_HIGH_PERFORMANCE
	if (strcmp((char *)Datagram->Header->Magic, "Art-Net") != 0)
	{
		free(DatagramBuffer);
		return FALSE;
	}
#endif
	Datagram->Header->ProtocolVersion = INTFlipByteOrder(Datagram->Header->ProtocolVersion);
	if (INTPacketDecodeFunctions[Datagram->Header->Opcode] == 0)
	{
		//TODO ADD LOG MESSAGE HERE
		free(DatagramBuffer);
		return FALSE;
	}
	if (!INTPacketDecodeFunctions[Datagram->Header->Opcode](Datagram, &DatagramBuffer[sizeof(ArtNetHeader)], DatagramLength - sizeof(ArtNetHeader)))
	{
		free(DatagramBuffer);
		return FALSE;
	}
	return TRUE;
}

BOOL ArtNetEncode(ArtNetDatagram* Datagram, uint8_t *Buffer, size_t *Length)
{
	assert(Datagram != 0 && Datagram->Header != 0);
	Datagram->Header->ProtocolVersion = INTFlipByteOrder(Datagram->Header->ProtocolVersion);
	if (INTPacketEncodeFunctions[Datagram->Header->Opcode] == 0)
	{
		//TODO ADD LOG MESSAGE HERE
		return FALSE;
	}
	memcpy(Buffer, Datagram->Header, sizeof(ArtNetHeader));
	*Length+=sizeof(ArtNetHeader);
	((ArtNetHeader*)Buffer)->ProtocolVersion = INTFlipByteOrder(((ArtNetHeader*)Buffer)->ProtocolVersion);
	if (!INTPacketEncodeFunctions[Datagram->Header->Opcode](Datagram, Buffer, Length))
	{
		return FALSE;
	}
	return TRUE;
}
