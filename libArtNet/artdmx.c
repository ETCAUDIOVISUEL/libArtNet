#include "stdafx.h"
#include "artdmx.h"
#include <assert.h>

typedef struct SINTArtDmxPacket {
	ArtNetHeader		  Header;
	ArtNetDMXPacketHeader PacketHeader;
} INTArtDMXPacket;

BOOL INTArtDMXDecode(ArtNetDatagram* Datagram, char* Payload, size_t PayloadLength)
{
	ArtNetDMXPacketHeader* Header;
	assert(Datagram != 0 && Datagram->Header != 0);
#ifndef ARTNET_HIGH_PERFORMANCE
	assert(Datagram->Header->Opcode == ARTNET_ARTDMX_OPCODE);
#endif
	if (PayloadLength < sizeof(ArtNetDMXPacketHeader))
	{
		return FALSE;
	}
	Header = (ArtNetDMXPacketHeader*) Payload;
	Datagram->PacketHeader = (uint8_t *) Payload;
	Header->Length = INTFlipByteOrder(Header->Length);
	if (Header->Length < (PayloadLength - sizeof(ArtNetDMXPacketHeader)))
	{
		return FALSE;
	}
	Datagram->Payload = (uint8_t *) &Payload[sizeof(ArtNetDMXPacketHeader)];
	return TRUE;
}

BOOL INTArtDMXEncode(ArtNetDatagram* Datagram, uint8_t *Buffer, size_t *Length)
{
	INTArtDMXPacket *Packet;
	assert(Datagram != 0 && Datagram->Header != 0);
#ifndef ARTNET_HIGH_PERFORMANCE
	assert(Datagram->Header->Opcode == ARTNET_ARTDMX_OPCODE);
#endif
	Packet = (INTArtDMXPacket *) Buffer;
	memcpy(&((uint8_t*)Packet)[sizeof(ArtNetHeader)], Datagram->PacketHeader, sizeof(ArtNetDMXPacketHeader));
	memcpy(&((uint8_t*)Packet)[sizeof(ArtNetHeader)+sizeof(ArtNetDMXPacketHeader)], Datagram->Payload, Packet->PacketHeader.Length);
	*Length += sizeof(ArtNetDMXPacketHeader) + Packet->PacketHeader.Length;
	Packet->PacketHeader.Length = INTFlipByteOrder(Packet->PacketHeader.Length);
	return TRUE;
}

void ArtDMXRegister()
{
	INTPacketDecodeFunctions[ARTNET_ARTDMX_OPCODE] = &INTArtDMXDecode;
	INTPacketEncodeFunctions[ARTNET_ARTDMX_OPCODE] = &INTArtDMXEncode;
}