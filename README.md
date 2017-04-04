# EECS_645_Project

This project will simulate reading an L1 cache.

The replacement algorithm used will be the LRU (Least Recently Used) algorithm.

To compile the program:
1. Go to command prompt.
2. Compile the program using the make command.

To run the program:
cache_simulator [-h][cache_size][associativity][block_size][trace file]

Parameter descriptions:
  - "-h" can be "ignored".
  - "cache_size" is the total size of the cache.
    - This value should be a power of 2.
    - Also, <cache_size> = (number of sets) * (set size) * <block_size>
  - "associativity" can be "direct", "assoc", or "assoc:n".
    - "direct" is used to simulate a direct mapped cache.
    - "assoc" is used to simulate a fully associative cache.
    - "assoc:n" is used to simulate an n-way associative cache. n should be a power of 2.
  - "trace_file" is the name of the binary trace file being used.
