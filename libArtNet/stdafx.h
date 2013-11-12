// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "artnet.h"

extern ArtNetPacketDecodeFunc INTPacketDecodeFunctions[0xFFFF];
extern ArtNetPacketEncodeFunc INTPacketEncodeFunctions[0xFFFF];

uint16_t INTFlipByteOrder( uint16_t val ) ;

// TODO: reference additional headers your program requires here
