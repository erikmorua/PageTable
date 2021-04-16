#include <stdio.h>
#include <stdlib.h>
#include "ptableHelper.h"

//total bytes allocated throughout the program
int bytesUsed = 0;
//creates and allocates pagetable struct memory and fills pagetable members.
int createPageTable(PAGETABLE *pageTable, char **argv, int pos){
    pageTable->shiftArr = calloc(pageTable->levelCount,sizeof(int));
	pageTable->bitMask = calloc(pageTable->levelCount, sizeof(unsigned int));
    pageTable->entryCount = calloc(pageTable->levelCount,sizeof(unsigned int));
    bytesUsed += sizeof(pageTable->shiftArr);
    bytesUsed += sizeof(pageTable->bitMask);
    bytesUsed += sizeof(pageTable->entryCount);
	int numBits = 0;
	int i;

    //loop number of levels and fill the shift array, bit mask array and entry count for each level
	for (i = 0; i < pageTable->levelCount; i++){
		int bitLevel = atoi(argv[pos]); // gives levels from entry 
		pageTable->shiftArr[i] = 32 - numBits - bitLevel;
		int startMask = 32 - numBits;
        pageTable->bitMask[i] = getBitmask(startMask,bitLevel);
		pageTable->entryCount[i] = (1 << bitLevel);
		pos++;
		numBits = numBits + bitLevel;

        if(numBits >= 32){ // error checking can't be > 32 bit system
            printf("Address length exceeds limit of 32 bits. \n");
            exit(1);
        }
	}
    bytesUsed += sizeof(numBits);
    //initialize pagetable variables
    pageTable->hits = 0;
    pageTable->misses = 0;
    pageTable -> rootnode = makeLevel(pageTable,pageTable->rootnode,0); // create new level
    return numBits;
}

//creates bit mask given a size
unsigned int getBitmask(int x, int y){
    unsigned int bitmask = (1 << y) - 1; 
    bitmask <<= (x-y);
    return bitmask;
}

//function will accept a pointer of type PAGETABLE, pointero f type LEVEL and an int, depth. Creates a new sublevel.
LEVEL * makeLevel(PAGETABLE *pageTable, LEVEL *level, int depth){
    level = calloc(1,sizeof(LEVEL));
    bytesUsed += sizeof(level);
    //setting variables passed in
    level->pageTable = pageTable;
    level->depth = depth;
    level->isLeaf = (depth+1 >= pageTable->levelCount);
    
    //if level isn't a root level, allocate memory
    if(level->isLeaf){
        level->map = calloc(pageTable->entryCount[depth],sizeof(MAP));
        bytesUsed += sizeof(level->map);
        int i;
        //set each of the levels to false 
        for(i=0; i < pageTable->entryCount[depth];i++){
            level->map[i].checkValid = false;
        }
    }
    else{
        //else create a new level
        level->nextlevel = calloc(pageTable->entryCount[depth], sizeof(LEVEL *));
        bytesUsed += sizeof(level->nextlevel);
    }
    return level;
}

MAP * PageLookup(PAGETABLE *PageTable, unsigned int LogicalAddress){
    return PageLookupHelp(PageTable->rootnode, LogicalAddress);
}

//Given a page table and logical address this function will return the approprate entry of the page table.
MAP * PageLookupHelp(LEVEL *levelptr, unsigned int LogicalAddress){
    unsigned int pos = LogicalToPage(LogicalAddress, levelptr->pageTable->bitMask[levelptr->depth], levelptr->pageTable->shiftArr[levelptr->depth]);
    bytesUsed += sizeof(pos);
    //found an existing entry
    if(levelptr->isLeaf){ // if levelptr is leaf, its not a new entry 
        if(levelptr->map[pos].checkValid){ // if its already been found checkValid will be true, if not then it will be false, create a new entry 
            return &levelptr->map[pos];
        }
        // found a new entry return null
        else{
            return NULL;
        }
    }
    //returns null if not found and creates a new entry
    else{
        if(levelptr->nextlevel[pos] == NULL){
            return NULL;
        }
        //if not null, its an entry, look up the entry at the logical address
        return PageLookupHelp(levelptr->nextlevel[pos],LogicalAddress);
    }
}
void PageInsert(PAGETABLE *PageTable, unsigned int LogicalAddress, unsigned int Frame){
	PageInsertHelp(PageTable->rootnode, LogicalAddress, Frame);
}

//this function will be used to add new entries to the page table when a page has not yet been allocated.
void PageInsertHelp(LEVEL *levelptr, unsigned int  LogicalAddress, unsigned int Frame){
    //finding position
    unsigned int pos = LogicalToPage(LogicalAddress, levelptr->pageTable->bitMask[levelptr->depth], levelptr->pageTable->shiftArr[levelptr->depth]);
    bytesUsed += sizeof(pos);
    if(levelptr->isLeaf){ //check it isn't a root node
        //set members of levelptr
		levelptr->map[pos].checkValid = true;
		levelptr->map[pos].frame = Frame;
		levelptr->pageTable->frameCount++;
	}
    else{
        //if there is no next nevel, make a new level
        if((levelptr->nextlevel[pos]) == NULL){
			levelptr->nextlevel[pos] = makeLevel(levelptr->pageTable, levelptr, levelptr->depth+1);
		}
        //insert page
		PageInsertHelp(levelptr->nextlevel[pos], LogicalAddress, Frame);
	}
}

//given a logical address, apply the given bit mask and shift right by the given number of bits. Returns the page number.
unsigned int LogicalToPage(unsigned int LogicalAddress, unsigned int bitMask, unsigned int shift){
    unsigned int page = LogicalAddress;
    page &= bitMask;
    page >>= shift;
    bytesUsed += sizeof(page);
    return page;
}

//IN: pointer to a pagetable Out: bitmask depending on the level 
void printBitMask(PAGETABLE *pageTable){
    int i;
	printf("Bitmasks\n");
	for (i = 0; i < pageTable->levelCount; i++) //create a bitmask for each of the levels
	{
		printf("level %i mask ", i);
        printf("%08X\n",pageTable->bitMask[i]);
	}
}
//Prints details about the Page Table. Accepts a pointer to pagetable, the number of Addresses in the pagetable and the offset.
void printSummary(PAGETABLE *pageTable, int numAddr, int offset){
    //calc the percentage
    float hitsPercent = (float)((pageTable->hits / numAddr) * 100);
    float missPercent = (float)((pageTable->misses / numAddr) * 100);
	int pagesize = (int) (pow(2,offset));
    //increments bytes
    bytesUsed += sizeof(hitsPercent);
    bytesUsed += sizeof(missPercent);
    bytesUsed += sizeof(pagesize);
	printf("Page size: %i bytes\n", pagesize);
	printf("Addresses processed: %i\n", numAddr);
	printf("Hits: %i (%.02f), Misses %i (%.02f)\n",pageTable->hits, hitsPercent, pageTable->misses, missPercent);
    printf("Frames allocated: %i\n", pageTable->frameCount);
	printf("Bytes used: %i\n", bytesUsed);
}