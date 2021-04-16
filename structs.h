#ifndef _structs_h
#define _structs_h

typedef enum {false,true} bool;

//Top level descriptor describing attributes of the N level page table. contains a pointer to the level 0 page structure.
typedef struct PAGETABLE{
	struct LEVEL *rootnode;
	int levelCount, frameCount;
	int hits, misses;
	int *entryCount;
	int *shiftArr;
	unsigned int *bitMask;

} PAGETABLE;

// entry for an arbitrary level, represents one of the sublevels
typedef struct LEVEL {
	int depth;
    bool isLeaf;
	struct PAGETABLE *pageTable;
	struct MAP *map;
	struct LEVEL **nextlevel;
} LEVEL;

// contains information about the mapping of a page to a frame. used in leaf nodes of tree.
typedef struct MAP{
	bool checkValid;
	unsigned int frame;
} MAP;


#endif