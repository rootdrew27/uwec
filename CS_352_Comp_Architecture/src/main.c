#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "structs.h"
#include "func.h"


int main(int argc, char *argv[])
{
    //filenames 
    char *config_file_name;
    char *trace_file_name;
    //load in names from CLI args
    config_file_name = argv[1];
    trace_file_name = argv[2];

    //file pointers
    FILE *config_fptr;
    FILE *trace_fptr;

    //open and validate files
    if ((config_fptr = fopen(config_file_name, "r")) == NULL)
    {
        printf("The config file (1st file) could not be opened\n");
        printf("Check the name and location of this file\n");
        exit(1);
    }
    if ((trace_fptr = fopen(trace_file_name, "r")) == NULL)
    {
        printf("The trace file (2nd file) could not be opened\n");
        printf("Check the name and location of this file\n");
        exit(1);
    } 
    
    //assign Cache Properties' values
    struct CacheProperties cacheProperties; 
    //read in config file (could be in function)
    fscanf(config_fptr, "%d", &(cacheProperties.line_size));
    fscanf(config_fptr, "%d", &(cacheProperties.associativity));
    fscanf(config_fptr, "%d", &(cacheProperties.cache_size));
    fscanf(config_fptr, "%d", &(cacheProperties.replacement_policy));
    fscanf(config_fptr, "%d", &(cacheProperties.miss_penalty));
    fscanf(config_fptr, "%d", &(cacheProperties.write_allocate_policy));
    //convert cache_size to bytes from KB
    cacheProperties.cache_size = 1000*cacheProperties.cache_size;
    //calculate num of cache blocks
    cacheProperties.num_of_cache_blocks = cacheProperties.cache_size/cacheProperties.line_size;

    //calculate address field sizes based on Cache Props
    setAddressFieldSizes(&cacheProperties);

    //create array of trace (memory access) structs
    int* num_of_ops_p = (int*)malloc(sizeof(int)); //used to get num of ops from creatMemAccessArray func
    struct MemoryAccess *arr_of_mem_accesses_objects = createMemAccessArray(trace_fptr, &cacheProperties, num_of_ops_p);

    //close files
    fclose(config_fptr); 
    fclose(trace_fptr);

    //initalize cache 
    struct CacheBlock *cache_arr = createCache(cacheProperties);

    //perform memory access ops
    struct Stats *stats;//assigned to the result of memory access ops...

    if (cacheProperties.associativity == 1)
    {
        stats = performMemoryAccessesDMC(arr_of_mem_accesses_objects, cache_arr, cacheProperties, *num_of_ops_p);
    }
    
    else if (cacheProperties.associativity == 0)
    { 
        stats = performMemoryAccessesFA(arr_of_mem_accesses_objects, cache_arr, cacheProperties, *num_of_ops_p);    
    }

    //adjust output file name
    char *output_file_name = strcat(trace_file_name, ".out");
    FILE *output_fp = fopen(output_file_name, "w");

    //print stats to file
    fprintf(output_fp, "%0.4f\n", (floor(stats->total_hit_rate*10000))/10000); //this truncs to 4 decimal places
    fprintf(output_fp, "%0.4f\n", stats->load_hit_rate);
    fprintf(output_fp, "%0.4f\n", stats->store_hit_rate);
    fprintf(output_fp, "%0.4f\n", stats->total_run_time);
    fprintf(output_fp, "%0.4f\n", stats->avg_mem_access_time);

    //close file
    fclose(output_fp);


    //free memory
    free(arr_of_mem_accesses_objects);
    free(cache_arr);
    free(stats);
    //terminate program
    return 0;
}