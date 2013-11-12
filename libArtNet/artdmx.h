#pragma once

#include <stdint.h>
#include "artnet.h"


#define ARTNET_ARTDMX_OPCODE 0x5000

typedef struct SArtNetDMXPacketHeader
{
	uint8_t  Sequence;
	uint8_t  Physical;
	uint16_t Universe;
	uint16_t Length;
} ArtNetDMXPacketHeader;

void ArtDMXRegister();