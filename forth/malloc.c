#include <stdlib.h>

typedef struct MemInfoStruct
{
	int Size;
	struct MemInfoStruct *Prev;		//prev block in line
	struct MemInfoStruct *Next;		//next block in line
	struct MemInfoStruct *PrevSize;		//previous block of same size
	struct MemInfoStruct *NextSize;		//next block of same size
} MemInfoStruct;

//first spot is 16 bytes, each spot higher is 2x larger than the last
//16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192
//last entry is a size/ptr combo
MemInfoStruct *MemBlocks[11];

#define IN_USE(x) (x->Size & 1)
#define SET_IN_USE(x) x->Size |= 1;
#define CLEAR_IN_USE(x) x->Size &= 0x7fffffe;

void init_memory(long addr, long size)
{
	MemInfoStruct *NewBlock;
	int i;

	//zero out MemBlocks
	memset(MemBlocks, 0, sizeof(MemBlocks));

	//create the new block and put it in position
	NewBlock = (MemInfoStruct *)addr;
	memset(NewBlock, 0, sizeof(MemInfoStruct));
	NewBlock->Size = size;
	MemBlocks[10] = NewBlock;
}

void *malloc(long size)
{
	MemInfoStruct *CurBlock;
	MemInfoStruct *NewBlock;
	int BlockID;
	int ExtraBlock;
	unsigned long n;
	

	//minimum 16 bytes for the block, add in our 9 bytes header to make sure we get a proper sized block
	size += (sizeof(int) * 3);

	//now make sure n has the low bit available to indicate in use
	if(size & 1)
		size++;

	//if there are bits set then increment up a block
	if(size & 0xf)
		n = (size >> 4) + 1;
	else
		n = size >> 4;

	//find the block we are in
	BlockID = 0;
	ExtraBlock = n & 1;
	for(n >>= 1; n && (BlockID < 10); BlockID++)
	{
		ExtraBlock |= (n & 1);
		n >>= 1;
	}

	//if more than 1 bit is set then bump a block level
	BlockID += ExtraBlock;

	//if in a bucket and it has an entry then return the first entry
	if(BlockID < 10)
	{
		if(MemBlocks[BlockID])
		{
			//we have an entry, grab it, move to the next entry and return it
			CurBlock = MemBlocks[BlockID];
			MemBlocks[BlockID] = MemBlocks[BlockID]->NextSize;

			//remove from the free chain, we get the first entry so no previous
			if(CurBlock->NextSize)
				CurBlock->NextSize->PrevSize = 0;
			CurBlock->PrevSize = 0;
			CurBlock->NextSize = 0;
			SET_IN_USE(CurBlock);
			return (void *)((unsigned int)CurBlock + (sizeof(int)*3));	//return pointer past the size and prev/next
		}

		//failed to find a block, set size to the next rounded size
		size = (1 << (BlockID + 4));
	}

	//cycle through the block list looking for one big enough
	CurBlock = MemBlocks[10];
	while(CurBlock)
	{
		if(CurBlock->Size >= size) {
			break;
		}
		CurBlock = CurBlock->NextSize;
	}

	//if no block then return
	if(!CurBlock)
		return 0;

	//see if the current block has enough room to be split
	//if not just return the block itself
	if(CurBlock->Size <= (size + 16))	//minimum block size is 16 that can be put into a bucket
	{
		//remove from memblocks if the first entry
		if(MemBlocks[10] == CurBlock)
			MemBlocks[10] = CurBlock->NextSize;

		//remove from the free chain
		if(CurBlock->PrevSize)
			CurBlock->PrevSize->NextSize = CurBlock->NextSize;
		if(CurBlock->NextSize)
			CurBlock->NextSize->PrevSize = CurBlock->PrevSize;
		CurBlock->PrevSize = 0;
		CurBlock->NextSize = 0;
		SET_IN_USE(CurBlock);

		CurBlock->Size = size;
		return (void *)((unsigned int)CurBlock + (sizeof(int)*3));	//return pointer past the size and prev/next
	}

	//split the block up and fixup linked lists
	NewBlock = (MemInfoStruct *)((int)CurBlock + size);
	NewBlock->Prev = CurBlock;
	NewBlock->Next = CurBlock->Next;
	if(NewBlock->Next)
		NewBlock->Next->Prev = NewBlock;
	CurBlock->Next = NewBlock;

	//set sizes
	NewBlock->Size = CurBlock->Size - size;
	CurBlock->Size = size;


	//fix up sizing linked list, if still in the large bucket then just adjust everything
	//otherwise we have to find a new bucket
	if(NewBlock->Size > 8192)
	{
		//fixup MemBlocks if need be
		if(MemBlocks[10] == CurBlock)
			MemBlocks[10] = NewBlock;

		//just update entries in the big block list
		NewBlock->PrevSize = CurBlock->PrevSize;
		if(NewBlock->PrevSize)
			NewBlock->PrevSize->NextSize = NewBlock;
		NewBlock->NextSize = CurBlock->NextSize;
		if(NewBlock->NextSize)
			NewBlock->NextSize->PrevSize = NewBlock;
	}
	else
	{
		//fixup MemBlocks if need be
		if(MemBlocks[10] == CurBlock)
			MemBlocks[10] = CurBlock->NextSize;

		//remove from our chain
		if(CurBlock->PrevSize)
			CurBlock->PrevSize->NextSize = CurBlock->NextSize;
		if(CurBlock->NextSize)
			CurBlock->NextSize->PrevSize = CurBlock->PrevSize;
		
		//find the chain to add to
		BlockID = 0;
		n = size >> 4;
		ExtraBlock = n & 1;
		for(n >>= 1; n && (BlockID < 10); BlockID++)
		{
			ExtraBlock |= (n & 1);
			n >>= 1;
		}

		//if more than 1 bit is set then bump a block level
		BlockID += ExtraBlock;
		if(BlockID > 10)
			BlockID = 10;

		//add it
		NewBlock->PrevSize = 0;
		NewBlock->NextSize = MemBlocks[BlockID];
		if(NewBlock->NextSize)
			NewBlock->NextSize->PrevSize = NewBlock;
		MemBlocks[BlockID] = NewBlock;
	}

	//make sure pointers are wiped out
	CurBlock->PrevSize = 0;
	CurBlock->NextSize = 0;

	//return CurBlock
	SET_IN_USE(CurBlock);
	return (void *)((unsigned int)CurBlock + (sizeof(int)*3));	//return pointer past the size and prev/next
}

void free(void *v)
{
	MemInfoStruct *CurBlock;
	MemInfoStruct *TempBlock;
	int BlockID;
	int ExtraBlock;
	int n;

	//move back into our header
	CurBlock = (MemInfoStruct *)((int)v - (sizeof(int) * 3));

	CLEAR_IN_USE(CurBlock);

	//see if we can bring the block ahead of us into our block
	TempBlock = CurBlock->Next;
	if(TempBlock && !(IN_USE(TempBlock)))
	{
		//we have a next block and not in use, combine
		CurBlock->Size += TempBlock->Size;
		CurBlock->Next = TempBlock->Next;
		if(TempBlock->Next)
			TempBlock->Next->Prev = CurBlock;

		//remove from it's free list
		if(TempBlock->PrevSize)
			TempBlock->PrevSize->NextSize = TempBlock->NextSize;
		if(TempBlock->NextSize)
			TempBlock->NextSize->PrevSize = TempBlock->PrevSize;

		//find the chain it is in and remove it if it is a header entry
		BlockID = 0;
		n = TempBlock->Size;

		//find the chain to remove from
		n >>= 4;
		ExtraBlock = n & 1;
		for(n >>= 1; n && (BlockID < 10); BlockID++)
		{
			ExtraBlock |= (n & 1);
			n >>= 1;
		}

		//if more than 1 bit is set then bump a block level
		BlockID += ExtraBlock;
		if(BlockID > 10)
			BlockID = 10;

		if(MemBlocks[BlockID] == TempBlock)
			MemBlocks[BlockID] = TempBlock->NextSize;
	}

	//see if we can combine with previous
	TempBlock = CurBlock->Prev;
	if(TempBlock && !(IN_USE(TempBlock)))
	{
		//we have a previous, adjust it and make it our block
		//we have a next block and not in use, combine
		TempBlock->Next = CurBlock->Next;
		if(CurBlock->Next)
			CurBlock->Next->Prev = TempBlock;

		//find the chain it is in and remove it if it is a header entry
		BlockID = 0;
		n = TempBlock->Size;

		//find the chain to remove from
		n >>= 4;
		ExtraBlock = n & 1;
		for(n >>= 1; n && (BlockID < 10); BlockID++)
		{
			ExtraBlock |= (n & 1);
			n >>= 1;
		}

		//if more than 1 bit is set then bump a block level
		BlockID += ExtraBlock;

		//update size after finding the original list the previous block is in
		TempBlock->Size += CurBlock->Size;

		//remove from it's free list if not a large block
		if(BlockID < 10)
		{
			if(TempBlock->PrevSize)
				TempBlock->PrevSize->NextSize = TempBlock->NextSize;
			if(TempBlock->NextSize)
				TempBlock->NextSize->PrevSize = TempBlock->PrevSize;

			if(MemBlocks[BlockID] == TempBlock)
				MemBlocks[BlockID] = TempBlock->NextSize;

			//need to insert the previous block back into the lists so make it current
			CurBlock = TempBlock;
		}
		else
			return;		//all done combining with previous and in the large list so done
	}

	//find the chain to add to
	BlockID = 0;
	n = CurBlock->Size;

	n >>= 4;
	ExtraBlock = n & 1;

	for(n >>= 1; n && (BlockID < 10); BlockID++)
	{
		ExtraBlock |= (n & 1);
		n >>= 1;
	}
	//if more than 1 bit is set then bump a block level
//	BlockID += ExtraBlock;

	if(BlockID > 10)
		BlockID = 10;


	//add it
	CurBlock->PrevSize = 0;
	CurBlock->NextSize = MemBlocks[BlockID];
	if(CurBlock->NextSize)
		CurBlock->NextSize->PrevSize = CurBlock;
	MemBlocks[BlockID] = CurBlock;

	return;
}
