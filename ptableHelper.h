#ifndef _ptableHelper_h
#define _ptableHelper_h

#include "structs.h"


int createPageTable(PAGETABLE *pageTable, char **argv, int pos);
unsigned int getBitmask(int x, int y);
LEVEL * makeLevel(PAGETABLE *pageTable, LEVEL *level, int depth);
MAP * PageLookup(PAGETABLE *PageTable, unsigned int LogicalAddress);
MAP * PageLookupHelp(LEVEL *levelptr, unsigned int LogicalAddress);
void PageInsert(PAGETABLE *PageTable, unsigned int LogicalAddress, unsigned int Frame);
void PageInsertHelp(LEVEL *levelptr, unsigned int LogicalAddress, unsigned int Frame);
unsigned int LogicalToPage(unsigned int LogicalAddress, unsigned int bitMask, unsigned int shift);
void printBitMask(PAGETABLE *pageTable);
void printSummary(PAGETABLE *pageTable, int numAddr, int offset);
#endif