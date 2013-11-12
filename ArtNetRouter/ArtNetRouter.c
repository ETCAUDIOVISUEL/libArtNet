// ArtNetRouter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "collections.h"
#include <ctype.h>
#include <string.h>
#pragma comment(lib, "libArtNet.lib")
#pragma comment(lib, "Ws2_32.lib")

WSADATA wsaData;

char* RemoveSpaces(const char *source, char *target) {
   do {
      if (!isspace(*source)) 
         *target++ = *source;
   } while(*source++);
   return target;
}

void ParseConfig(IntListTable **RoutingTable){
	FILE *ConfigFile;
	char *Line = (char *) malloc(255);
	char *Token;
	int Key, Value;
	IntLinkedList *SubUniverse;
	IntLinkedList *Entry;
	if (fopen_s(&ConfigFile,"routerconf.cfg", "r") != 0){
		printf("%i\n",errno);
		return;
	}
	while ((Line = fgets(Line, 255, ConfigFile)) != 0) 
	{
		Line = strtok(Line, "#");
		Token = strtok(Line, "=");
		Key = atoi(Token);
		SubUniverse = CreateIntLinkedList();
		while((Token = strtok(NULL, "=,")) != 0) {
			Entry = (IntLinkedList *) malloc(sizeof(IntLinkedList));
			Entry->Value = atoi(Token); 
			AddLast(SubUniverse, Entry);
		}
		Put(RoutingTable, Key, SubUniverse);
	}
	free(Line);
	fclose(ConfigFile);
}


IntLinkedList *GetRemoteEndPoints(IntListTable **RoutingTable,int Universe)
{
	IntLinkedList *Temp;
	IntLinkedList *Temp2;
	IntLinkedList *RemoteEndPoints = CreateIntLinkedList();
    IntLinkedList *SubUniverses = Get(RoutingTable, Universe);
	if (SubUniverses != 0) {
        for (Temp = SubUniverses->Next; Temp != SubUniverses; Temp = Temp->Next)
        {
			Temp2 = GetRemoteEndPoints(RoutingTable, Temp->Value);
			Concatenate(RemoteEndPoints, Temp2);
			FreeList(Temp2);
        }
    } else {
        if (Universe < 256) {//Universe 255 is excluded
			Temp2 = (IntLinkedList *) malloc(sizeof(IntLinkedList));
			Temp2->Value = Universe;
            AddLast(RemoteEndPoints, Temp2);
		}
    }
    return RemoteEndPoints;
}


int _tmain(int argc, _TCHAR* argv[])
{
	uint8_t Sequence = 0;
	int iResult;
	size_t OutLength;
	uint8_t *Buffer;
	SOCKET ReceiveSock;
	SOCKET TransmitSock;
	SOCKADDR_IN addr,            
               remote_addr, to_addr;  
	ArtNetDMXPacketHeader *Header;
	ArtNetDatagram Datagram;
	IntListTable *RoutingTable[KEY_RESOLUTION];
	IntLinkedList *RemoteEndPoints, *CurrentEndPoint;
	unsigned long mode = 1;
    int iRemoteAddrLen = sizeof(SOCKADDR_IN); 
	memset(RoutingTable,0,sizeof(IntListTable)*KEY_RESOLUTION);
	ParseConfig(RoutingTable);
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	OutLength = 0;
	ArtDMXRegister();
    ReceiveSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    TransmitSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ioctlsocket(TransmitSock,FIONBIO,&mode);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(16454);
	to_addr.sin_family = AF_INET;
	to_addr.sin_addr.S_un.S_un_b.s_b1 = 192;
	to_addr.sin_addr.S_un.S_un_b.s_b2 = 168;
	to_addr.sin_addr.S_un.S_un_b.s_b3 = 42;
	to_addr.sin_port = htons(6454);
	if (bind(ReceiveSock, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("ERROR BINDING TO PORT 16454\n");
		exit(-1);
	}
	Buffer = (uint8_t *) malloc(ARTNET_MAX_PACKET_SIZE);
	while (TRUE) {
		if (ArtNetReceive(ReceiveSock, &Datagram, (SOCKADDR *)&remote_addr, &iRemoteAddrLen)){
			//printf ( "ARTNET RECEIVED OPCODE:%x %x\n",Datagram.Header->Opcode,Sequence);
			if (Datagram.Header->Opcode == ARTNET_ARTDMX_OPCODE) {
				Header = (ArtNetDMXPacketHeader *) Datagram.PacketHeader;
				Header->Sequence = Sequence++;
				if (Sequence > 255)
					Sequence = 0;
				RemoteEndPoints = GetRemoteEndPoints(RoutingTable, Header->Universe);
				Header->Universe = 0;
				OutLength = 0;
				if (ArtNetEncode(&Datagram, Buffer, &OutLength)) {
					for (CurrentEndPoint = RemoteEndPoints->Next; CurrentEndPoint != RemoteEndPoints; CurrentEndPoint = CurrentEndPoint->Next) 
					{
						to_addr.sin_addr.S_un.S_un_b.s_b4 = (UCHAR) CurrentEndPoint->Value;
						sendto(TransmitSock, (char *) Buffer, OutLength, 0, (SOCKADDR *) &to_addr, sizeof(SOCKADDR_IN));
					}
				}
				FreeList(RemoteEndPoints);
			}
			free(Datagram.Header);
		}
	}
	return 0;
}
