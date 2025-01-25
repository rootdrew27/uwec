Cache Simulator

CS352 Computer Architecture

Andrew Root

Notes:
    - This implementation assumes 32bit addresses
    - Memory efficiency could be improved (in structs for example) because readability and writability were prioritized 
    - Replacement Policy is ignored for Direct Mapped Cache (as it should be)
    - I could have used Typedef w/ structs but it was more readable for me (a novice C programmer) to have 'struct' there each time
    - Certain memory operations will take more clock cycles due to the control logic used to implement them. This makes direct comparison difficult.
    - Lots of parameters had to be passed. (perhaps structs should have been linked to other structs (eg CacheBlock could have had a pointer to CacheProperties (this would likely increase speed but likely require more mem)))
    - The MemoryAccess struct is very inefficient in terms of memory space. The tag and index could be calcultaed when the operation is occuring rather than while reading in from the file, and thus they would not need to be stored
    - Code is not modular
    - The logic is sepreated fairly well. perhaps an additional function file would be wise.

    - !!!This implementation requires the user to understand the significance of the values that they put in the Config file!! (eg. if Fully Associative is set, then the line size and cache size must be equal. It may produce a result but this result would not be representative of a Fully Associative cache) (eg the cache size must not exceed the line size) these actions have undefined behavior
    - This implementation assumes that the Fully Associative config will result in a 0 bit length for index size (as previously alluded to)
    - possibly need to change naming convention to distinguish vars from parameters
    - I regret this implementation. I should have made 6 funcitons which corresponded to each possible         configuration to further seperate the logic. My hope was to make the code scalable to eventually be able to simulate N-way cache, but I did not follow through with this
    - memory is not freed well..
    - if i had time I would have created trace files to clearly test whether or not setting write-allocate worked... but I am confident in the logic that I implemented

ALGO

Input: two CL arguments. The Config file and the Trace file
Output: a file "trace_file_name.out" containing stats

Begin
    - Open & Validate files.
    - Read data into appropriate structs.
    - Calculate the sizes of the Tag Field, Index Field, and Byte Select Field
    - Create array of mem_access_objects and calculate the tag and index for each
    - Based on Cache properties, call a function with the 'acceses' array (data from trace file) as a parameter.  

    Direct Mapped Cache  
        Loop through array of 'acceses':
            Given an 'address' & 'access type' --> Determine the Tag, Index, and Select Byte(omit Select Byte for this project)
            IF 'access type' == load 
                THEN: check cache block associated with index of 'address' 
                IF 'valid flag' == 0
                    THEN: Cache Miss
                        This would be when you access main memory 
                        Update tag and valid flag of cache block (Stall the program for a set amount of operations to simulate mem access)
                IF 'valid flag' == 1 AND addresses match:
                    THEN: Done!
                        This is where you would load from cache
                IF 'valid flag' == 1 BUT addresses DO NOT match
                    THEN: this is a miss (main memory must be accessed and this data will be overwritten)

            IF 'access type' == store
                IF 'valid flag' == 0 AND 'write-allocate' == 1 
                    THEN: Load memory into cache block and then write to it
                
                IF 'valid flag' == 0 AND 'write-allocate' == 0
                    THEN: write directly to memory

                IF 'valid flag' == 1
                    THEN: write to cache block 



    
    Fully Associative Cache
        IF 'Replacement Policy' == 1 
            THEN: keep track for cache block access history
        Keep track of Access History
        Loop through array of 'acceses':
            Given an 'address' & 'access type' 
            IF 'access type' == load 
                THEN: Loop through all cache blocks
                    IF 'valid flag' == 0 
                        THEN: Continue
                    IF 'valid flag' == 1 BUT addresses DO NOT match:
                        THEN: Continue
                     
                    IF 'valid flag' == 1 AND addresses DO match
                        THEN: Cache Hit (break inner loop)
                    
                    IF Loop ends naturally:
                            THEN: Do Replacement (check policy) (MISS)
   
                IF 'access type' == store
                    THEN: Loop through all cache blocks
                        IF 'address' is matched
                            THEN: Cache Hit! (break inner loop)
                                  
                        ELSE (MISS)
                            IF 'write allocate' == 1:
                                THEN: Do Replacement (check policy) 

                            ELSE 'write allocate' == 0 
                                THEN: Write to Main Memory 
                        

End


Considerations
    Disregard first cache miss for each index (compulsory miss)

