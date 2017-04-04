
/** Cache Simulator written in Cache **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cache_simulator.h"

/**
	Pointer to allow for function emulating
**/
CacheLine *makeColdCache()
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
/**
	Set number of bits needed for the set index and block offset
**/
void setSubs()
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
		h_setSub++;
	}
	/** Checks after for-loop to validate **/
	if((i != (h_numLines / h_associativity)) && h_setSub != 0)
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
	tagLengthResult = strlen(address) - h_setSub - h_blockSub;

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
/** Performs a write-through or "wt" read operation on a read in address. **/
void readCache(char address[])
{
	int index = getIndex(address, getTagLength(address)) * h_associativity;
	int highestIndex = index;
	int highest = 0;

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

	index = getIndex(address, getTagLength(address)) * h_associativity;
	i = index;
	for(; i < index + h_associativity; i++)
	{
		if(h_someCache[i].h_valid == 0)
		{
			h_someCache[i].h_valid = 1;
			h_someCache[i].h_tagLength = getTagLength(address);
			strcpy(h_someCache[i].h_tag, address);
			updateRecents(i, h_someCache[i].h_set);

			return;
		}
	}

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
	char *c;
	char ip[20];
	char address[20];
	FILE *traceFile; /** fstream variable equivalent **/

	if(strcmp(argv[1], "-h") == 0)
	{
        printf("Usage: c-sim [-h] <cache size> <associativity> <block size> <write policy> <trace file> \n < cachesize > is the total size of the cache. This should be a power of 2. Also, it should always be true that < cachesize > = number of sets × < setsize > × < blocksize >. \n    For direct-mapped caches, < setsize > = 1. For n − way associative caches, < setsize > = n. ");
		printf("\n    Given the above formula, together with < cachesize >, < setsize >, and < blocksize >, you can always compute the number of sets in your cache. \n < associativity > is one of: \n     – direct - simulate a direct mapped cache. \n");
		printf("     – assoc - simulate a fully associative cache. \n     – assoc:n - simulate an n − way associative cache. n should be a power of 2. \n < blocksize > is an power of 2 integer that specifies the size of the cache block. \n < writepolicy > is one of: \n     – wt - simulate a write through cache. \n     – wb - simulate a write back cache. \n < tracefile > is the name of a file that contains memory access traces. \n");
        return 0;
    }

	/** Check if input matches amount of required parameters **/
	if(argc < 5 || argc > 5)
	{
        printf("ERROR: Usage: c-sim [-h] <cache size> <associativity> <block size> <trace file> \n");

        return 1;
    }

	/** Cache size parameter **/
	h_size = atoi(argv[1]); /** atoi converts String to integer **/

	/** Block size parameter **/
	h_blockSize = atoi(argv[3]);

	/** Initialize h_associativity **/
	/** Calculations depend on cache size and block size **/
	h_associativity = 0;

	/** Check associativity input **/
	/** Remove/replace to implement integer input **/
	if(strcmp(argv[2],"direct") == 0)
	{
		h_associativity = 1;
	}
	else if(strcmp(argv[2],"assoc") == 0)
	{
		h_associativity = h_size / h_blockSize;
	}
	else
	{
		c = strtok(argv[2], ":"); /** Breaks String/arg into tokens using delimiter ":" **/
		c = strtok(NULL, ":");
		if(c == NULL)
		{
			printf("ERROR: invalid associativity\n");

			return 1;
		}
		h_associativity = atoi(c);
	}

	/** Open trace file **/
	traceFile = fopen(argv[4], "r");
	if(traceFile == NULL)
	{
		fprintf(stderr, "ERROR: cannot open input file.\n");

		return 1;
	}

	h_numLines = h_size / h_blockSize;
	h_someCache = makeColdCache();
	if(h_numLines / h_associativity * h_associativity * h_blockSize != h_size || h_size == 0)
	{
		fprintf(stderr, "ERROR: invalid size, block size, or associativity. Cache size must greater than 0 and equal to num_sets * assoc * block size \n");

		return 1;
	}

	setSubs();

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
    	printf("Memory writes: %d\n", h_memory_writes);

	fclose(traceFile);
  	freeCache();

	return 0;

}
