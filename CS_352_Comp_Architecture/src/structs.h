#ifndef STRUCTS_H
#define STRUCTS_H

#include <time.h>
//there is one Cache properties instance per program execution 
//cacheProperites instance can be accessed throughout (this increases code dependency) but increases writability as well
struct CacheProperties 
{
    int line_size; //in bytes
    int cache_size; //in bytes
    int num_of_cache_blocks;

    int miss_penalty; //1 or 0
    int associativity; //1 or 0 
    int replacement_policy; //1 or 0
    int write_allocate_policy;//1 or 0

    struct AddressFieldSizes
    {
        int tag_size; //in bits
        int index_size; //in bits
        int byte_select_size; //used in bit extract function
    }address_field_sizes;  
};



struct CacheBlock
{
    int index;
    int valid_bit;
    uint32_t tag;
    //data in cache would go here
};

struct MemoryAccess
{
    long address;
    int num_instr; //number of instructions ran since the last memory access 
    char access_type; 
    struct AddressFields
    {
        uint32_t tag; 
        int index;
        //int byte_select;
    }address_fields;
};

struct Stats
{
    double total_hit_rate; //4 decimal truncation
    double load_hit_rate; //4 deciaml precision
    double store_hit_rate; //4 decimal precision
    double total_run_time; //in cycles
    double avg_mem_access_time; //cycles per mem access (4 decimal precision)
};



#endif