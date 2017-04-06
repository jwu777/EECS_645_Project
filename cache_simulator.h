
#ifndef cache_simulator_h
#define cache_simulator_h

/**
	Make a cache, with every line being invalid
**/
/** typedef added "clean up" code and reduce repetition of "struct" **/
typedef struct CacheLine
{
	int h_valid;
	char *h_tag;
	char *h_block;
	int h_set;
	int h_recent;
	int h_dirty;
	int h_tagLength;

} CacheLine; /** struct variable **/

int h_cache_hits = 0;
int h_cache_misses = 0;
int h_memory_reads = 0;

int h_numLines = 0;
int h_size = 0;
int h_blockSize = 0;
int h_associativity = 0;

int h_blockSub = 0;
int h_setSub = 0;

CacheLine* h_someCache;

CacheLine *makeInvalidCache(); /** Fill CacheLine struct with invalid cache **/

/**
	Initialization/tag methods - EDIT
**/
void setSubs();
int getTagLength(char address[]);
int getIndex(char address[], int tagLength);
void updateRecents(int new, int index);

/**
	Cache read policy
**/
void readCache(char address[]);

/**
	Utility/deallocation methods
**/
void printCache();
void freeCache();

#endif
