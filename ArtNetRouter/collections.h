#pragma once
#include <stdint.h>

#define LL_HANDLE_FLAG (1<<1)
#define KEY_RESOLUTION 100

typedef struct SIntLinkedList {
	struct SIntLinkedList *Next;
	struct SIntLinkedList *Prev;
	int Value;
	int Flags;
} IntLinkedList;

typedef struct SIntListTable {
	struct SIntListTable *Next;
	int Key;
	struct SIntLinkedList *Value;
} IntListTable;

void AddLast(IntLinkedList *List, IntLinkedList *Entry);

void AddFirst(IntLinkedList *List, IntLinkedList *Entry);

typedef void (*IteratorDelegate)(IntLinkedList *Entry);

void Iterate(IntLinkedList *List, IteratorDelegate Delegate);

void Concatenate(IntLinkedList *List, IntLinkedList *List2);

int Size(IntLinkedList *List);

void FreeList(IntLinkedList *List);

IntLinkedList *CreateIntLinkedList();

void Put(IntListTable **Table, int Key, IntLinkedList *Value);

IntLinkedList *Get(IntListTable **Table, int Key);