#ifndef LEVEL_H
#define LEVEL_H

struct LEVEL {
    
    int depth;
    struct PAGETABLE *pageTable;
    struct LEVEL **nextLevel;
    struct MAP *map;
    bool isLeafNode;
    
};

#endif