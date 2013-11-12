#include "collections.h"
#include "stdafx.h"
#include <stdlib.h>

void AddLast(IntLinkedList *List, IntLinkedList *Entry)
{
	Entry->Next = List;
	Entry->Prev = List->Prev;
	List->Prev->Next = Entry;
	List->Prev = Entry;
}

void AddFirst(IntLinkedList *List, IntLinkedList *Entry)
{
	Entry->Next = List->Next;
	Entry->Prev = List;
	List->Next->Prev = Entry;
	List->Next = Entry;
}

void Iterate(IntLinkedList *List, IteratorDelegate Delegate)
{
	IntLinkedList *Current;
	for (Current = List->Next; Current != List; Current = Current->Next)
			Delegate(Current);
}

void Concatenate(IntLinkedList *List, IntLinkedList *List2)
{
	if (List2->Next == List2)
		return;
	List2->Next->Prev = List;
	List2->Prev->Next = List;
	List->Prev->Next = List2->Next;
	List->Prev = List2->Prev;
}

int Size(IntLinkedList *List)
{
	int Counter = 0;
	IntLinkedList *Current;
	for (Current = List->Next; Current != List; Current = Current->Next)
		Counter++;
	return Counter;
}

IntLinkedList *CreateIntLinkedList()
{
	IntLinkedList *Instance = (IntLinkedList *) malloc (sizeof(IntLinkedList));
	Instance->Prev = Instance;
	Instance->Next = Instance;
	Instance->Value = 0;
	Instance->Flags = LL_HANDLE_FLAG;
	return Instance;
}

void Put(IntListTable **Table, int Key, IntLinkedList *Value)
{
	IntListTable *Entry = (IntListTable *) malloc(sizeof(IntListTable));
	Entry->Key = Key;
	Entry->Value = Value;
	Entry->Next = 0;
	if (Table[Key % KEY_RESOLUTION] != 0) 
	{
		Table[Key % KEY_RESOLUTION]->Next = Entry;
	} else {
		Table[Key % KEY_RESOLUTION] = Entry;
	}
}

IntLinkedList *Get(IntListTable **Table, int Key)
{
	IntListTable *Current;
	IntListTable *List = Table[Key % KEY_RESOLUTION];
	if (List == 0)
		return 0;
	if (List->Key == Key)
		return List->Value;
	for (Current = List->Next; Current != 0; Current = Current->Next)
		if (List->Key == Key)
			return List->Value;
	return 0;		
}

void FreeList(IntLinkedList *List)
{
	IntLinkedList *Current, *Next;
		Current = List;
	while (1) {
		Next = Current->Next;
		free(Current);
		if (Current == List)
			return;
		Current = Next;
	}
}