#ifndef PAGETABLE_H
#define PAGETABLE_H

#include "level.h"
#include "map.h"

#define ADDRESS_LENGTH 32 // Using 32 bit addresses

struct PAGETABLE {
    
    int levelCount;
    unsigned int *bitmaskArray;
    int *shiftArray;
    int *entryCount;
    struct LEVEL *rootNode;
    int frameCount;
    
};

unsigned int logicalToPage(unsigned int logicalAddress, unsigned int mask, unsigned int shift);

MAP * pageLookup(PAGETABLE *pageTable, unsigned int logicalAddress);

MAP * pageLookupRecursion(LEVEL *level, unsigned int logicalAddress);

void pageInsert(PAGETABLE *pageTable, unsigned int logicalAddress, unsigned int frame);

void pageInsertRecursion(LEVEL *level, unsigned int logicalAddress, unsigned int frame);

int getBitsUsed(PAGETABLE *pageTable, char *argv[], int levelIndex);

unsigned int createBitmask(int start, int length);

LEVEL * createLevel(PAGETABLE *pageTable, int depth);

unsigned int getPhysicalAddress(unsigned int frame, unsigned offset, unsigned int bitsUsed);

unsigned int* getPages(PAGETABLE* pageTable, unsigned int logicalAddress);

unsigned int getPageSize(unsigned int bitsUsed);

#endif