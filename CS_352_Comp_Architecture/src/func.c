#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "structs.h"
#include "func.h"


//standard method for determining field sizes
void setAddressFieldSizes(struct CacheProperties *cache_props)
{
    int L, M; 
    M = log2(cache_props->cache_size);
    L = log2(cache_props->line_size);

    struct AddressFieldSizes addressFieldSizes;
    addressFieldSizes.byte_select_size = L;
    addressFieldSizes.index_size = M - L;
    addressFieldSizes.tag_size = 32 - M; 

    cache_props->address_field_sizes.tag_size = addressFieldSizes.tag_size;
    cache_props->address_field_sizes.index_size = addressFieldSizes.index_size;
    cache_props->address_field_sizes.byte_select_size = addressFieldSizes.byte_select_size;

    return;
}
//returns decimal number from a give range of the input (as if it were in binary) (This function is dervied from GeeksForGeeks)
int extractDecimalFromBits(int number, int num_of_bits, int start_position)
{
    return (((1 << num_of_bits) - 1) & (number >> (start_position - 1)));
}


//count the num of mem_access ops

//read in line by line and assign to struct members (realloc strcut array if needed)
//returns array of memory accesses
struct MemoryAccess* createMemAccessArray(FILE *fp, struct CacheProperties *cache_props, int *num_of_ops_p)
{
    int step_size = 5;
    int realloc_limit = step_size;
    struct MemoryAccess *array_of_mem_accesses = (struct MemoryAccess*)malloc(step_size*sizeof(struct MemoryAccess));
    char buffer[30];
    int index = 0;
    while(fgets(buffer, 30, fp) != NULL)
    {
        //check if additional memory is needed 
        if(index >= realloc_limit)
        {
            array_of_mem_accesses = realloc(array_of_mem_accesses, (realloc_limit+step_size)*sizeof(struct MemoryAccess));
            realloc_limit += step_size;
        }
        char *token = strtok(buffer, " ");  
        array_of_mem_accesses[index].access_type = *token;
        token = strtok(NULL, " ");
        //covert to long
        char *temp; //to preform strtol function
        array_of_mem_accesses[index].address = strtol(token, &temp, 16);
        token = strtok(NULL, " ");
        array_of_mem_accesses[index].num_instr = atoi(token);
        memset(buffer, 0, sizeof(buffer)); //reset buffer
        calcAddressFields(&(array_of_mem_accesses[index]), cache_props); 
        index++;
    }

    *num_of_ops_p = index;
    return array_of_mem_accesses;
}

//using the 'address' member of a mem_access, calculate the 'address fields' (ie the tag/index/byte_select)
void calcAddressFields(struct MemoryAccess *mem_access, struct CacheProperties *cache_props)
{
    if(cache_props->associativity == 1)//calcuate index and tag
    {
        mem_access->address_fields.index = extractDecimalFromBits(mem_access->address, cache_props->address_field_sizes.index_size , cache_props->address_field_sizes.byte_select_size + 1);
        mem_access->address_fields.tag = extractDecimalFromBits(mem_access->address, cache_props->address_field_sizes.tag_size, cache_props->address_field_sizes.byte_select_size + cache_props->address_field_sizes.index_size + 1);
    }
    else //index is ignored for Fully Associative Cache
    {
        mem_access->address_fields.tag = extractDecimalFromBits(mem_access->address, cache_props->address_field_sizes.tag_size, cache_props->address_field_sizes.byte_select_size);
    }
    

}

//create array of empty cache blocks
struct CacheBlock* createCache(struct CacheProperties cache_props)
{

    struct CacheBlock *cache_block_arr = (struct CacheBlock*)malloc(cache_props.num_of_cache_blocks*sizeof(struct CacheBlock));
    //initialize cache blocks
    for(int i = 0; i < cache_props.num_of_cache_blocks; i++)
    {
        cache_block_arr[i].index = i;//this is irrelevant for Fully associative
        cache_block_arr[i].valid_bit = 0;
        cache_block_arr[i].tag = 0;// this works because addresses wont get checked until valid bit is set (ie data is loaded in)
    }
    return cache_block_arr;
}

struct Stats *performMemoryAccessesDMC(struct MemoryAccess* mem_access_arr, struct CacheBlock* cache_arr, struct CacheProperties cache_props, int num_of_ops)
{
    //declare structs stats, and sub-stats (used to calculate stats)
    int mem_access_op_count = num_of_ops;
    int load_count = 0;
    int store_count = 0;
    int load_hit_count = 0;
    int store_hit_count = 0;
    clock_t start_t, end_t;
    int miss_penalty = 0; //sum of miss penalties (to be added to total run time)
    int miss_penalty_increment = cache_props.miss_penalty;

    struct Stats *stats_p = (struct Stats*)malloc(sizeof(struct Stats));

    struct MemoryAccess *current_mem_access; 
    struct CacheBlock *current_cache_block;

    start_t = clock();
    //loop through mem_access_arr 
    for(int i = 0; i < num_of_ops; i++) 
    {
        //set the following vars to make comparisions more efficient
        current_mem_access = &mem_access_arr[i];
        int index_of_mem_access = current_mem_access->address_fields.index;
        current_cache_block = &cache_arr[index_of_mem_access];
        uint32_t tag_of_mem_access = current_mem_access->address_fields.tag;
        int valid_bit = current_cache_block->valid_bit;

        if(current_mem_access->access_type == 'l')
        {
            load_count++;

            if (valid_bit == 1)
            {
                if (tag_of_mem_access == current_cache_block->tag)
                {
                    load_hit_count++;
                }
                else //miss
                {
                    miss_penalty += miss_penalty_increment;
                    //load data in from mem
                    //ie set the cache block's valid_bit/tag
                    current_cache_block->tag = current_mem_access->address_fields.tag;
                    current_cache_block->valid_bit = 1;
                }
            }
            else if (valid_bit == 0) //miss
            {
                miss_penalty += miss_penalty_increment;
                //load data in from mem 
                //ie set the cache block's valid_bit/tag
                current_cache_block->tag = current_mem_access->address_fields.tag;
                current_cache_block->valid_bit = 1;        
            }
        }

        else if(current_mem_access->access_type == 's')
        {
            store_count++;

            if (valid_bit == 1) 
            {
                if(current_cache_block->tag == tag_of_mem_access)
                {
                    //write to cache block
                    store_hit_count++;
                }
                else if (cache_props.write_allocate_policy == 1) //miss
                {
                    miss_penalty += miss_penalty_increment;
                    current_cache_block->tag = tag_of_mem_access;
                }
                else //miss
                {
                    //write directly to mem
                    miss_penalty += miss_penalty_increment;
                }           
            }
            if (valid_bit == 0) //miss
            {
                miss_penalty += miss_penalty_increment;

                if(cache_props.write_allocate_policy == 1)
                {
                    //load mem into cache block
                    current_cache_block->tag = tag_of_mem_access;
                    current_cache_block->valid_bit = 1;
                }
                //else write directly to mem
            }
        }
    }
    end_t = clock();
    //calc stats
    stats_p->total_hit_rate = (double)(store_hit_count + load_hit_count)/mem_access_op_count;
    stats_p->load_hit_rate = (double)load_hit_count/load_count;
    stats_p->store_hit_rate = (double)store_hit_count/store_count;
    stats_p->total_run_time = (double)(end_t- start_t + miss_penalty);
    stats_p->avg_mem_access_time = (double)(stats_p->total_run_time)/mem_access_op_count;

    return stats_p;
} 

void updateAgeBits(int *age_status_arr, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (age_status_arr[i] == 0)
        {
            age_status_arr[i] = length - 1;
        }
        else
        {
            age_status_arr[i]--;
        }
    }
}

int getLRUBitIndex(int *age_status_arr, int length)
{
    for(int i = 0; i < length; i++)
    {
        if (age_status_arr[i] == 0)
        {
            return i;
        }
    }
    printf("error occured in getLRUBitIndex func");
    exit(1);
}


struct Stats *performMemoryAccessesFA(struct MemoryAccess* mem_access_arr, struct CacheBlock* cache_arr, struct CacheProperties cache_props, int num_of_ops)
{
        //declare structs stats, and sub-stats (used to calculate stats)
    int mem_access_op_count = num_of_ops;
    int load_count = 0;
    int store_count = 0;
    int load_hit_count = 0;
    int store_hit_count = 0;
    clock_t start_t, end_t;
    int miss_penalty = 0; //sum of miss penalties (to be added to total run time)
    int miss_penalty_increment = cache_props.miss_penalty;

    //create pointer to Stats object
    struct Stats *stats_p = (struct Stats*)malloc(sizeof(struct Stats));

    struct MemoryAccess *current_mem_access;
    struct CacheBlock current_cache_block;

    int *age_status_arr; //may not be used, but this statement is needed to compile
    
    //if needed, create array to keep track of replacement policy
    if(cache_props.replacement_policy == 1)
    {   
        age_status_arr = (int*)malloc(cache_props.num_of_cache_blocks*sizeof(int));
        //set status of age bits
        for (int i = 0; i < cache_props.num_of_cache_blocks; i++)
        {
            age_status_arr[i] = i;
        }
    }
    start_t = clock();
    //loop through mem_access_arr 
    for(int i = 0; i < num_of_ops; i++) 
    {
        //set the following vars to make comparisions more efficient
        current_mem_access = &mem_access_arr[i];
        
        if (current_mem_access->access_type == 'l')
        {
            load_count++;
            int j;
            //loop through cache blocks to compare tags
            for(j = 0; j < cache_props.num_of_cache_blocks; j++)
            {   
                current_cache_block = cache_arr[j];

                if(current_cache_block.valid_bit == 1)
                {
                    if(current_mem_access->address_fields.tag == current_cache_block.tag)
                    {
                        load_hit_count++;
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            
            }
            //if address was not matched and the array was full traversed
            if (j == cache_props.num_of_cache_blocks)
            {
                //miss
                miss_penalty += miss_penalty_increment;
                //set LRU cache block to address tag of current mem access op
                int index = getLRUBitIndex(age_status_arr, cache_props.num_of_cache_blocks);
                cache_arr[index].tag = current_mem_access->address_fields.tag;
                //set valid bit
                cache_arr[index].valid_bit = 1;
                //update age statuses
                updateAgeBits(age_status_arr, cache_props.num_of_cache_blocks);
            }
        }

        if (current_mem_access->access_type == 's')
        {
            store_count++;
            int j;      
            //loop through cache blocks to compare tags
            for(j = 0; j < cache_props.num_of_cache_blocks; j++)
            {   
                current_cache_block = cache_arr[j];

                if(current_cache_block.valid_bit == 1)
                {
                    if(current_mem_access->address_fields.tag == current_cache_block.tag)
                    {
                        store_hit_count++;
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            
            }
            //if array is traversed completely and write policy is set to 'write-allocate' then...
            if(j == cache_props.num_of_cache_blocks)
            {   
                //miss
                miss_penalty += miss_penalty_increment;
                if(cache_props.write_allocate_policy == 1)
                {
                    //if address was not matched (ie the array was fully traversed) and using LRU
                    if (cache_props.replacement_policy == 1)
                    {
                        //set LRU cache block to address tag of current mem access op
                        int index = getLRUBitIndex(age_status_arr, cache_props.num_of_cache_blocks);
                        cache_arr[index].tag = current_mem_access->address_fields.tag;
                        //set valid bit
                        cache_arr[index].valid_bit = 1;
                        //update age statuses
                        updateAgeBits(age_status_arr, cache_props.num_of_cache_blocks);
                    }
                    else //do random replacement 
                    {
                        int random_num = rand() % cache_props.num_of_cache_blocks;
                        cache_arr[random_num].tag = current_mem_access->address_fields.tag; 
                    }
                }
                //else write directly to memory
            }   
        }
    }
    end_t = clock();
    
    //create array to manage replacement policy

    //calc stats
    stats_p->total_hit_rate = (double)(store_hit_count + load_hit_count)/mem_access_op_count;
    stats_p->load_hit_rate = (double)load_hit_count/load_count;
    stats_p->store_hit_rate = (double)store_hit_count/store_count;
    stats_p->total_run_time = (double)(end_t - start_t + miss_penalty);
    stats_p->avg_mem_access_time = (double)(stats_p->total_run_time)/mem_access_op_count;

    return stats_p;
}

