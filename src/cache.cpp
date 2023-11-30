#include "cache.h"

CACHE::CACHE(cache_type_t type, uint16_t size, uint16_t entry_size, cache_replacement_policy_t cache_policy, uint32_t cache_block_size){
    // initialize cache
    policy = cache_policy;
    cache.set_count = size;
    cache.type = type;
    cache.block_size = cache_block_size;
    if(cache.type == DIRECT_MAPPED){
        cache.entry_size = 1; // for direct mapped cache
    }
    else if(cache.type == FULLY_ASSOCIATIVE){
        cache.entry_size = cache.set_count; // for fully associative cache
    }
    else if(cache.type == SET_ASSOCIATIVE){
        cache.entry_size = entry_size; // for set associative cache
    }
    // initialize cache sets
    cache.sets = (cache_set_t *)calloc(sizeof(cache_set_t) * cache.set_count, sizeof(cache_set_t));
    // initialize cache sets
    for(int i = 0; i < cache.set_count; i++){
        cache.sets[i].entries_count = 0;
        cache.sets[i].size = cache.entry_size;
        cache.sets[i].entries = (cache_entry_t *)calloc(sizeof(cache_entry_t) * cache.sets[i].size, sizeof(cache_entry_t));
        for(int j = 0; j < cache.sets[i].size; j++){
            cache.sets[i].entries[j].data = (uint32_t *)calloc(sizeof(uint32_t) * cache_block_size, sizeof(uint32_t));
            cache.sets[i].entries[j].modified = false;
            cache.sets[i].entries[j].valid = false;
        }
    }
    // initialize cache stats
    for(int i=0;i< (cache.block_size >> MEM_BIT_LEN);i++){
        cache.offset_mask = cache.offset_mask << 1;
        cache.offset_mask = cache.offset_mask | 1;
    }
    cache_stats.num_hits = 0;
    cache_stats.num_misses = 0;
}
        
data_ret_t CACHE::get_data_direct_mapped(uint32_t addr){

    // printf("[Cache] Getting data\n");
    uint8_t offset = addr & cache.offset_mask;
    uint8_t set_idx = (addr >> cache.block_size >> MEM_BIT_LEN) & (cache.set_count - 1);
    uint32_t tag = addr >> ((cache.block_size >> MEM_BIT_LEN) + count_bits(cache.set_count - 1));
    
    data_ret_t ret;
    // printf("[Cache] Offset: %d Set idx: %d Tag: %d\n", offset, set_idx, tag);

    if(cache.sets[set_idx].entries[0].valid == true && cache.sets[set_idx].entries[0].tag == tag){
        cache_stats.num_hits++;
        // printf("[Cache] Hit %d\n",cache_stats.num_hits);
        ret.data = cache.sets[set_idx].entries[0].data[offset];
        ret.mem_fault = false;
        return ret;
    }
    else{
        cache_stats.num_misses++;
        // printf("[Cache] Miss %d\n",cache_stats.num_misses);
        cache.sets[set_idx].entries[0].valid = true;
        cache.sets[set_idx].entries[0].tag = tag;
        cache.sets[set_idx].entries[0].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
        ret.data = cache.sets[set_idx].entries[0].data[offset];
        ret.mem_fault = true;
        // printf("[Cache] Miss %d\n",cache_stats.num_misses);
        return ret;
    }
}

data_ret_t CACHE::get_data_set_associative(uint32_t addr){
    uint8_t offset = addr & cache.offset_mask;
    uint8_t set_idx = (addr >> (cache.block_size >> MEM_BIT_LEN)) & (cache.set_count - 1);
    uint32_t tag = addr >> ((cache.block_size >> MEM_BIT_LEN) + count_bits(cache.set_count - 1));

    data_ret_t ret;

    // printf("[Cache] Getting data tag %d ; addr %d ; countbits %d\n", tag, addr, count_bits(cache.set_count - 1));
    for(int i = 0; i < cache.sets[set_idx].entries_count; i++){
        if(cache.sets[set_idx].entries[i].valid == true && cache.sets[set_idx].entries[i].tag == tag){
            cache_stats.num_hits++;
            if(policy == LRU){
                // move entry to front
                // printf("[Cache] Hit %d\n",cache_stats.num_hits);
                // for (int j=0;j<cache.sets[set_idx].entries_count;j++){
                //     printf("%d ",cache.sets[set_idx].entries[j].tag);
                // }
                // printf("\n");
                cache_entry_t temp = cache.sets[set_idx].entries[i];
                for(int j = i; j < cache.entry_size - 1 ; j++){
                    cache.sets[set_idx].entries[j] = cache.sets[set_idx].entries[j+1];
                }
                // printf("After reordering \n");
                cache.sets[set_idx].entries[cache.entry_size-1] = temp;
                ret.data = cache.sets[set_idx].entries[i].data[offset];
                // for (int j=0;j<cache.sets[set_idx].entries_count;j++){
                //     printf("%d ",cache.sets[set_idx].entries[j].tag);
                // }
                // printf("\n");
            }
            else{
                ret.data = cache.sets[set_idx].entries[i].data[offset];
            }
            ret.mem_fault = false;
            return ret;
        }
    }
    cache_stats.num_misses++;
    // if cache is full
    if(cache.sets[set_idx].entries_count == cache.sets[set_idx].size){
        // replace random entry
        if (policy == FIFO || policy == LRU){
            uint8_t idx = cache.entry_size - 1;
            // printf("################# [Cache] Miss %d %d\n",cache_stats.num_misses,tag);
            // for (int j=0;j<cache.sets[set_idx].entries_count;j++){
            //     printf("%d ",cache.sets[set_idx].entries[j].tag);
            // }
            // printf("\n");
            for(int i=0 ; i < cache.entry_size - 1; i++){
                cache.sets[set_idx].entries[i] = cache.sets[set_idx].entries[i+1];
            }
            cache.sets[set_idx].entries[idx].valid = true;
            cache.sets[set_idx].entries[idx].tag = tag;
            cache.sets[set_idx].entries[idx].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
            // printf("After replacement \n");
            // for (int j=0;j<cache.sets[set_idx].entries_count;j++){
            //     printf("%d ",cache.sets[set_idx].entries[j].tag);
            // }
            // printf("\n");
            ret.data = cache.sets[set_idx].entries[idx].data[offset];
            ret.mem_fault = true;
            return ret;
        }
        else { // RANDOM
            uint8_t idx = rand() % cache.sets[0].size;
            cache.sets[set_idx].entries[idx].valid = true;
            cache.sets[set_idx].entries[idx].tag = tag;
            cache.sets[set_idx].entries[idx].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
            ret.data = cache.sets[set_idx].entries[idx].data[offset];
            ret.mem_fault = true;
            return ret;
        }
    }
    else{
        // add new entry
        // printf("[Cache] Miss %d %d\n",cache_stats.num_misses,tag);
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].valid = true;
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].tag = tag;
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
        cache.sets[set_idx].entries_count++;
        ret.data = cache.sets[set_idx].entries[cache.sets[set_idx].entries_count - 1].data[offset];
        ret.mem_fault = true;
        return ret;
    }
}   

data_ret_t CACHE::get_data(uint32_t addr){
    if(cache.type == DIRECT_MAPPED){
        return get_data_direct_mapped(addr);
    }
    else{
        return get_data_set_associative(addr);
    }
}


data_ret_t CACHE::exec(uint32_t addr, uint32_t data ,access_type_t access_type){
    return get_data(addr);
}

void CACHE::print_stats(){
    printf("[CACHE] hits: %d misses: %d\n", cache_stats.num_hits, cache_stats.num_misses);
}

CACHE::~CACHE(){
    for(int i = 0; i < cache.set_count; i++){
        free(cache.sets[i].entries);
    }
    free(cache.sets);
}
