#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "pagetable.h"

// Given a logical address, apply the given bit mask and shift right by the given number of bits
unsigned int logicalToPage(unsigned int logicalAddress, unsigned int mask, unsigned int shift) {
    
    unsigned int page = logicalAddress;
    page &= mask; // Bitwise AND assignment operator
    page >>= shift; // Right shift AND assignment operator
    
    return page; // Return page number
    
}

// Given a page table and a logical address, return the appropriate entry of the page table. 
MAP * pageLookup(PAGETABLE *pageTable, unsigned int logicalAddress) {
    
    return pageLookupRecursion(pageTable->rootNode, logicalAddress);
    
}

MAP * pageLookupRecursion(LEVEL *level, unsigned int logicalAddress) {
    
    unsigned int bitmaskArray = level->pageTable->bitmaskArray[level->depth];
    unsigned int shiftArray = level->pageTable->shiftArray[level->depth];
    unsigned int pageIndex = logicalToPage(logicalAddress, bitmaskArray, shiftArray);
    
    if (level->isLeafNode) { // Only leaf nodes have a valid frame index
        if (level->map[pageIndex].isValid) {
            return &level->map[pageIndex];
        }
        else {
            return NULL;
        }
    }
    else {
        if (level->nextLevel[pageIndex] == NULL) { 
            return NULL;
        }
    }
    
    return pageLookupRecursion(level->nextLevel[pageIndex], logicalAddress); // Traverse to next level
    
}

// Add new entries to the page table when we have discovered that a page has not yet been allocated
void pageInsert(PAGETABLE *pageTable, unsigned int logicalAddress, unsigned int frame) {
    
    pageInsertRecursion(pageTable->rootNode, logicalAddress, frame);
    
}

void pageInsertRecursion(LEVEL *level, unsigned int logicalAddress, unsigned int frame) {
    
    unsigned int bitmaskArray = level->pageTable->bitmaskArray[level->depth];
    unsigned int shiftArray = level->pageTable->shiftArray[level->depth];
    unsigned int pageIndex = logicalToPage(logicalAddress, bitmaskArray, shiftArray);
    
    if (level->isLeafNode) { // Only leaf nodes have a valid frame index
        level->map[pageIndex].isValid = true;
        level->map[pageIndex].frame = frame;
        level->pageTable->frameCount++;
    }
    else { // Traverse to the next level. Create a new level if nextLevel[index] is NULL
        if (level->nextLevel[pageIndex] == NULL) {
            level->nextLevel[pageIndex] = createLevel(level->pageTable, level->depth + 1);
        }
        pageInsertRecursion(level->nextLevel[pageIndex], logicalAddress, frame);
    }
    
}

// Initialize page table and return bitsUsed
int getBitsUsed(PAGETABLE *pageTable, char **argv, int levelIndex) {
    
    pageTable->bitmaskArray = (unsigned int*) calloc(pageTable->levelCount, sizeof(unsigned int));
    pageTable->shiftArray = (int*) calloc(pageTable->levelCount, sizeof(int));
    pageTable->entryCount = (int*) calloc(pageTable->levelCount, sizeof(int));
    
    int bitsUsed = 0;
    
    for (int i = 0; i < pageTable->levelCount; i++) {
        int levelBits = atoi(argv[levelIndex]); // Bits used for current level
        pageTable->shiftArray[i] = ADDRESS_LENGTH - bitsUsed - levelBits;
        int maskStart = ADDRESS_LENGTH - bitsUsed; // Calculate bitmask for current level
        pageTable->bitmaskArray[i] = createBitmask(maskStart, levelBits);
        pageTable->entryCount[i] = pow(2, levelBits);
        bitsUsed += levelBits;
        levelIndex++;
    }
    
    pageTable->rootNode = createLevel(pageTable, 0);
    
    return bitsUsed;
    
}

unsigned int createBitmask(int start, int length) {
    
    unsigned int bitmask = pow(2, length) - 1; 
    bitmask <<= (start - length); // Shift the 1's to the correct starting position
    
    return bitmask;
    
}

// Allocate space for a new level in the page table
LEVEL * createLevel(PAGETABLE *pageTable, int depth) {
    
    LEVEL *level = (LEVEL*) calloc(1, sizeof(LEVEL));
    level->pageTable = pageTable;
    level->depth = depth;
    level->isLeafNode = (depth + 1 >= pageTable->levelCount);
    
    if (level->isLeafNode) { // Allocate maps for leaf nodes
        level->map = (MAP*) calloc(pageTable->entryCount[depth], sizeof(MAP));
    }
    else { // Allocate next level pointers
        level->nextLevel = (LEVEL**) calloc(pageTable->entryCount[depth], sizeof(LEVEL *));
    }
    
    return level;
    
}

unsigned int getPhysicalAddress(unsigned int frame, unsigned offset, unsigned int bitsUsed) {
    
    return frame << (ADDRESS_LENGTH - bitsUsed) | offset;
    
}

unsigned int* getPages(PAGETABLE* pageTable, unsigned int logicalAddress) { // Create and return pages array
    
    unsigned int* pages = (unsigned int*) calloc(pageTable->levelCount, sizeof(unsigned int *));

    for (int i = 0; i < pageTable->levelCount; i++) {
        pages[i] = logicalToPage(logicalAddress, pageTable->bitmaskArray[i], pageTable->shiftArray[i]);
    }
    
    return pages;
    
}

unsigned int getPageSize(unsigned int bitsUsed) {
    
    unsigned int offsetLength = ADDRESS_LENGTH - bitsUsed;
    unsigned int pageSize = pow(2, offsetLength);
    
    return pageSize;
    
}