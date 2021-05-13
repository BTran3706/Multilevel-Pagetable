#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include "pagetable.h"

using namespace std;

extern "C" {
#include "byutr.h"
#include "output_mode_helpers.h"
}

bool nFlag = false, oFlag = false;

// Check for optional n and o arguments and get # of levels from arguments
void parseArguments(int argc, char** argv, unsigned int* addressLimit, char** outputMode, int* levelCount) {

    if (argc < 2) {
        cout << "Minimum 2 arguments are required." << endl;
        exit(1);
    }
    
    int options;
    *levelCount = argc - 2; // pagetable and file name are required arguments so levelCount is argc - 2 if no optional arguments

    while ((options = getopt(argc, argv, "n:o")) != -1) {
        switch (options) {
            case 'n':
                nFlag = true;
                *addressLimit = atoi(optarg); // addressLimit is argument after -n
                *levelCount -= 2; // -n N are 2 additional arguments so subtract 2 to get levelCount
                break;
            case 'o':
                oFlag = true;
                *outputMode = optarg; // outputMode is argument after -o
                *levelCount -= 2; // -o mode are 2 additional arguments so subtract 2 to get levelCount
                break;
            default:
                break;
        }
    }
    
}

void executePagetable(int argc, char** argv, int addressLimit, char* outputMode, int levelCount) {

    FILE *traceFile;
    p2AddrTr traceAddress;
  
    int traceFileIndex = argc - (levelCount + 1); // traceFileIndex is always right before the 1st level so subtract levelCount + 1
    
    if ((traceFile = fopen(argv[traceFileIndex], "rb")) == NULL) { // File doesn't exist
        cout << "Cannot open file." << endl;
        exit(1);
    }
	
    PAGETABLE *pageTable = (PAGETABLE*) calloc(1, sizeof(PAGETABLE));
    unsigned int bitsUsed = getBitsUsed(pageTable, argv, argc - levelCount);
    unsigned int addressCount, totalBytes, hits, misses;
	
    while (!feof(traceFile)) {
        if (nFlag && addressCount >= addressLimit) { // If nFlag, process only up to the specified addressLimit
            break;
        }
        if (NextAddress(traceFile, &traceAddress)) {
            addressCount++;
            unsigned int logicalAddress = (unsigned int) traceAddress.addr;
			if (pageLookup(pageTable, logicalAddress) == NULL) { // Page has not been seen yet
                misses++;
				pageInsert(pageTable, logicalAddress, pageTable->frameCount);  // Insert the new page
                totalBytes += sizeof(LEVEL);
            }
            else {
                hits++;
            }
            if (oFlag && !strcmp(outputMode, "bitmasks")) { // Write out bitmasks for each level starting with highest level
                report_bitmasks(pageTable->levelCount, pageTable->bitmaskArray);     
            }
            MAP*  map = pageLookup(pageTable, logicalAddress);
            if (oFlag) {
                unsigned int offset = (logicalAddress << bitsUsed) >> bitsUsed;
                if (strcmp(outputMode, "logical2physical")) {
                    unsigned physicalAddress = getPhysicalAddress(map->frame, offset, bitsUsed);
                    report_logical2physical(logicalAddress, physicalAddress); // Each logical address is mapped to physical address
                }
                else if (strcmp(outputMode, "page2frame")) {
                    unsigned int* pageArray = getPages(pageTable, logicalAddress);
                    report_pagemap(logicalAddress, pageTable->levelCount, pageArray, map->frame); // Page # for each level shown followed by frame #
                }
                else if (strcmp(outputMode, "offset")) {
                    report_logical2offset(logicalAddress, offset); // Show mapping between logical addresses and their offset
                }
				else if (strcmp(outputMode, "summary")) {
                    unsigned int pageSize = getPageSize(bitsUsed);
                    report_summary(pageSize, hits, addressCount, map->frame, totalBytes); // Summary statistics
                }
            }
			else {
                unsigned int pageSize = getPageSize(bitsUsed);
                report_summary(pageSize, hits, addressCount, map->frame, totalBytes); // Default with no -o argument is report_summary 
            }
        }
    }

    fclose(traceFile);
    
}

int main (int argc, char** argv) {

    char* outputMode;
    int levelCount;
    unsigned int addressLimit;

    parseArguments(argc, argv, &addressLimit, &outputMode, &levelCount);
    executePagetable(argc, argv, addressLimit, outputMode, levelCount);

    return 0;
    
}