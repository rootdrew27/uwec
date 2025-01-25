#ifndef FUNC_H
#define FUNC_H

//use cache properties to determine the Address field sizes 
void setAddressFieldSizes(struct CacheProperties *cache_props);
//create C objects for each memory access operation (from the trace file) -> return a pointer to the array of ops
struct MemoryAccess *createMemAccessArray(FILE *fp, struct CacheProperties *cache_props, int *num_of_ops);
//extracts a decimal from a section of a binary number -> returns decimal value
int extractDecimalFromBits(int number, int num_of_bits, int position);
// calculates address field for a particular memory access operation's address  
void calcAddressFields(struct MemoryAccess *mem_access, struct CacheProperties *cache_props);
// initialize cache blocks -> return a pointer to the array of cache blocks
struct CacheBlock *createCache(struct CacheProperties cache_props);
//perfrom trace file ops (cache is Direct Mapped Cache) -> pointer to stats struct which are calculated in func
struct Stats *performMemoryAccessesDMC(struct MemoryAccess* mem_access_arr, struct CacheBlock* cache_arr, struct CacheProperties cache_props, int num_of_ops);
//used in LRU to update the age_status_array
void updateAgeBits(int *age_status_arr, int length);
// get LRU bit's index -> index of LRU bit
int getLRUBitIndex(int *age_status_arr, int length);
//perfrom trace file ops (cache is Fully Associative) -> pointer to stats struct which are calculated in func
struct Stats *performMemoryAccessesFA(struct MemoryAccess* mem_access_arr, struct CacheBlock* cache_arr, struct CacheProperties cache_props, int num_of_ops);

#endif