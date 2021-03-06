
/** Cache Simulator written in C **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cache_simulator.h"

/**
	Pointer to allow for function emulating.
	Generates an "empty" cache.
**/
CacheLine *makeInvalidCache()
{
	CacheLine *cache;

	int i;
	i = 0;

	cache = malloc(sizeof(CacheLine) * h_numLines);

	for(; i < h_numLines; i++)
	{
		cache[i].h_valid = 0;
		cache[i].h_tag = (char*)malloc(sizeof(char) * 81);
		strcpy(cache[i].h_tag, "-");
		cache[i].h_set = i / h_associativity;
		cache[i].h_recent = 0;
		cache[i].h_dirty = 0;
		cache[i].h_tagLength = 1;
	}
	return cache;
}

/**
	Initialization/tag methods - EDIT
**/

/** Sets the number of bits for the set index and block offset **/
void setBits()
{
	int i = 1;
	for(; i < h_blockSize; i*=2)
	{
		h_blockSub++;
	}
	/** Checks after for-loop to validate **/
	if((i != h_blockSize) && (h_blockSub != 0))
	{
		printf("ERROR: Block size is not a power of 2.\n");
		exit(1);
	}

	i = 1;
	for(; i < h_numLines / h_associativity; i*=2)
	{
		h_setBit++;
	}
	/** Checks after for-loop to validate **/
	if((i != (h_numLines / h_associativity)) && h_setBit != 0)
	{
		printf("ERROR: Associativity is not a power of 2.\n");
		exit(1);
	}

}

/** Get the tag length of each address using the block size and number
of sets **/
int getTagLength(char address[])
{
	int tagLengthResult = 0;
	tagLengthResult = strlen(address) - h_setBit - h_blockSub;

	return tagLengthResult;
}

/** Conversion to binary inputs not required? **/

/** Get the set index from binary address **/
int getIndex(char address[], int tagLength)
{
	int index = 0;
	int exp = 1;
	int i = strlen(address - 1 - h_blockSub);

	for(; i >= tagLength; i--)
	{
		if(address[i] == '1')
		{
			index += exp;
		}
		exp = exp << 1; /** Left-shift operator **/
	}
	return index;
}

/**
	Replacement algorithm
**/

/** Least Recently Used Replacement Algorithm **/
/** Updates the most recently used set to find the least used line **/
void updateRecents(int new, int index)
{
	int i = index * h_associativity;
	for(; i < index * h_associativity + h_associativity; i++)
	{
		h_someCache[i].h_recent++;
	}
	h_someCache[new].h_recent = 0;
}

/**
	Cache read policy
**/

/** Performs a read operation on a read in address. **/
void readCache(char address[])
{
	/** Calculate the set index **/
	int index = getIndex(address, getTagLength(address)) * h_associativity;
	int highestIndex = index;
	int highest = 0;

	/** Check set for a line where there is a tag match  **/
	int i = index;
	for(; i < index + h_associativity; i++)
	{
		if(h_someCache[i].h_valid == 1)
		{
			if(strncmp(h_someCache[i].h_tag, address, h_someCache[i].h_tagLength) == 0)
			{
				h_cache_hits++;
				updateRecents(i, h_someCache[i].h_set);

				return;
			}
		}
	}
	h_cache_misses++;
	h_memory_reads++;

	/** Look again through set for a non-valid line **/
	index = getIndex(address, getTagLength(address)) * h_associativity;
	i = index;
	for(; i < index + h_associativity; i++)
	{
		if(h_someCache[i].h_valid == 0)
		{			
			h_someCache[i].h_valid = 1;
			
			/** Reads data into line from lower memory **/
			/** Gives a memory read **/
			h_someCache[i].h_tagLength = getTagLength(address);
			strcpy(h_someCache[i].h_tag, address);
			updateRecents(i, h_someCache[i].h_set);

			return;
		}
	}

	/** Looks to evict the last used line in the set **/
	index = getIndex(address, getTagLength(address)) * h_associativity;
	i = index;
	for(; i < index + h_associativity; i++)
	{
		if(h_someCache[i].h_recent > highest)
		{
			highest = h_someCache[i].h_recent;
			highestIndex = i;
		}
	}
	
	/** Reads data into line from lower memory **/
	/** Gives a memory read **/
	h_someCache[highestIndex].h_tagLength = getTagLength(address);
	strcpy(h_someCache[highestIndex].h_tag, address);
	updateRecents(highestIndex, h_someCache[highestIndex].h_set);
}

/**
	Utility/deallocation methods
**/

/** Prints the cache **/
void printCache()
{
	int i = 0;
	int j = 0;

	for(; i < h_numLines; i++)
	{
		printf("index: %d set: %d valid: %d recent: %d tag length: %d tag:", i, h_someCache[i].h_set, h_someCache[i].h_valid, h_someCache[i].h_recent, h_someCache[i].h_tagLength);
		for(; j < h_someCache[i].h_tagLength; j++)
		{
			printf("%c", h_someCache[i].h_tag[j]);
		}
		printf("\n");
	}
}

/** Frees memory allocated to cache **/
void freeCache()
{
	int i = 0;
	for(; i < h_numLines; i++)
	{
		free(h_someCache[i].h_tag);
	}
}

/**
	Main method

	Tests and runs the simulator
**/

int main(int argc, const char * argv[])
{
	char ip[20];
	char address[20];
	FILE *traceFile; /** fstream variable equivalent **/

	printf("Usage: cache-simulator <cache size> <associativity> <block size> <trace file> \n");

	if(strcmp(argv[1], "-h") == 0)
	{
		/** assoc:n **/
        
        /**return 0;**/
    }

/**TEST**/
printf("Command arguments: %d\n",argc);

	/** Check if input matches amount of required parameters **/
	if(argc < 5 || argc > 5)
	{
        printf("ERROR: Usage: cache-simulator <cache size> <associativity> <block size> <trace file> \n");

        return 1;
    }

	/** Cache size parameter **/
	h_size = atoi(argv[1]); /** atoi converts String to integer **/

/**TEST**/
printf("Cache size: %d\n",h_size);

	/** Block size parameter **/
	h_blockSize = atoi(argv[3]);

/**TEST**/
printf("Block size: %d\n",h_blockSize);

	/** Initialize h_associativity **/
	/** Calculations depend on cache size and block size **/
	h_associativity = 0;

	/** Check associativity input **/
	h_associativity = atoi(argv[2]);

/**TEST**/
printf("Associativity: %d\n",h_associativity);

	if(h_associativity > 5)
	{
		printf("ERROR: High associativity\n");

		return 1;
	}

	/** Open trace file **/
	traceFile = fopen(argv[4], "r");

/**TEST**/
printf("Trace file: %s\n",argv[4]);

	if(traceFile == NULL)
	{
		fprintf(stderr, "ERROR: cannot open input file.\n");

		return 1;
	}

	h_numLines = h_size / h_blockSize;
	h_someCache = makeInvalidCache();
	if(h_numLines / h_associativity * h_associativity * h_blockSize != h_size || h_size == 0)
	{
		fprintf(stderr, "ERROR: invalid size, block size, or associativity. Cache size must greater than 0 and equal to num_sets * assoc * block size \n");

		return 1;
	}

	setBits();

	/**
		TRACE FILE READ - EDIT
	**/

	while(fscanf(traceFile, "%s %s", ip, address) != EOF)
	{
		/** Check eof from ip variable **/

		readCache(address);
	}


	printf("Cache hits: %d\n", h_cache_hits);
    	printf("Cache misses: %d\n", h_cache_misses);
    	printf("Memory reads: %d\n", h_memory_reads);

	fclose(traceFile);
  	freeCache();

	return 0;

}
