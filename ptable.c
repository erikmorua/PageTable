/*
Erik Morua, cssc3333, Red ID: 816670615
Nathan Azoulay, cssc3302 Red ID: 821871239
Program 3 Multilevel Page Table
The program will construct a simulation of an N-level Page tree. This program will read a file consisting of memory accesses for a single process, construct the tree 
and assign frame indices to each page. The user will indicate how many bits are to be used for each of the page table levels, and a user-specified file containing hexadecimal addresses 
will be used to construct a page table.
Included: ptable.c, ptableHelper.c, ptableHelper.h, structs.h, byutr.h, byu_tracereader.c, MAKEFILE
USEAGE: make 'To compile' 
        pagetable ~/lib/cs570/trace/trace.sample.tr 8 12

Pair Programming Equitable Participation & Honesty Affidavit
We the undersigned promise that we have in good faith attempted to follow the principles of pair programming. Although we were free to discuss ides with others,
the implementation is our own. We have shared a common workspace and taken turns at the keyboard for the majority of the work that
we are submitting. Furthermore, any non programming portions of the assignment were done independently. We recognize that should this not be the case,
we will be subject to penalties as outlined in the course syllabus.

Pair Programmer 1  Nathan Azoulay, Red ID: 821871239, 3/25/2021
Pair Programmer 2  Erik Morua, Red ID: 816670615, 3/25/2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "structs.h"
#include "ptableHelper.h"
#include "byutr.h"

bool N_FLAG = false, O_FLAG = false;
int limit;
char mode[50];
int levelCount;

//parses the command line entry and looks for flags
void parseLine(int argc, char ** argv){
    levelCount = argc-2;
    int fp;
    while((fp = getopt(argc,argv,"n:o")) != -1){
        switch(fp){
            case 'n': //procceses only the first N memory references. processes all if not present
                levelCount = levelCount - 2;
                N_FLAG = true;
                limit = atoi(optarg);
                break;
            case 'o': //sets output mode, will work depending on the mode set
                O_FLAG = true;
                levelCount = levelCount - 2;
                if(N_FLAG){
					strcat(mode,argv[4]);
					strcat(mode, "\0");
				}
				else{
					strcat(mode,argv[2]);
					strcat(mode, "\0");
				}
                break;
            default:
                break;
        }
    }
}

//creates a page table of size based on input, runs through file addresses checking for the mode.
void pageSimulation(int argc, char **argv){
    FILE *fTrace;
	int offset = 0;
    int filePos = 0;
	PAGETABLE *pageTable = calloc(1, sizeof(PAGETABLE));
	pageTable->levelCount = levelCount;
    filePos = levelCount - 1;
    offset = 32 - createPageTable(pageTable,argv,(argc-levelCount));
    //printf("Offset: %i\n", offset);
    p2AddrTr traceAddr;

    //error checking the file
    if((fTrace = fopen(argv[argc-filePos-2],"rb")) == NULL){
        fprintf(stderr, "Cannot open file %s", argv[argc-filePos-2]);
        exit(1);
    }

    unsigned long numAddr = 0;
    while(!feof(fTrace)){
        if(N_FLAG){
            if(numAddr >= limit) 
                break;
        }

        if(NextAddress(fTrace,&traceAddr)){
			unsigned int currAddr = (unsigned int)traceAddr.addr;
            if(PageLookup(pageTable,currAddr) == NULL){ // Page not seen yet
                pageTable->misses++;
                PageInsert(pageTable,currAddr,pageTable->frameCount);//insert new page at current address when a page hasnt been found
            }
            else{
                //reads the times a page if found in the table
                pageTable->hits++;
            }
            //page numbers for each level shown followed by the frame number
            if(strcmp(mode,"page2frame")==0){
                    unsigned int p = 00000000;
                    unsigned int tmp = currAddr;
                    currAddr = currAddr >> 12;
                    p+=currAddr;
                    //p = p >> 20;

					MAP *map = PageLookup(pageTable,tmp);

                    printf("%08X: ", tmp);
                    printf("%01X -> %01X\n",p,map->frame);
			}
			//show mapping between logical addresses and their offset
            if(strcmp(mode,"offset")==0){
                unsigned int p = 00000000;
                unsigned int tmp = currAddr;
                currAddr = currAddr << 20;
                p+= currAddr;
                p = p >> 20;
                printf("%08X -> %08X\n", tmp, p);
			} 
            //logical address is mapped to a physical address
			if(strcmp(mode,"logical2physical")==0){
                MAP *map = PageLookup(pageTable,currAddr);
                unsigned int physical = currAddr;
                physical &= ((1 << offset) - 1);
                physical += (map->frame << (offset));
                printf("%08X ", currAddr);
                printf("-> %08X\n", physical);
            }   
		}
        //increments total addresses
        numAddr++;
            
    }
    //write out bitmasks for each level starting with the highest level, one per line.
    if(strcmp(mode,"bitmasks")==0){
        printBitMask(pageTable);
    }
	//prints summary statistics. default argument if -o is not used 
    if(strcmp(mode,"summary")==0){
        printSummary(pageTable, numAddr,offset);
	}
	fclose(fTrace);
}

//driver
int main(int argc, char **argv){
    parseLine(argc,argv);
    pageSimulation(argc,argv);

    return 0;
}
