#include "cache.h"

CACHE::CACHE(cache_type_t type, uint16_t size, uint16_t entry_size, cache_replacement_policy_t cache_policy){
    // initialize cache
    policy = cache_policy;
    cache.set_count = size;
    cache.type = type;

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
    }
    // initialize cache stats
    cache_stats.num_hits = 0;
    cache_stats.num_misses = 0;
}
        
data_ret_t CACHE::get_data_direct_mapped(uint32_t addr){

    // printf("[Cache] Getting data\n");
    uint8_t offset = addr & CACHE_OFFSET_MASK;
    uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
    uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));
    
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
    uint8_t offset = addr & CACHE_OFFSET_MASK;
    uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
    uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));

    data_ret_t ret;

    for(int i = 0; i < cache.sets[set_idx].entries_count; i++){
        if(cache.sets[set_idx].entries[i].valid == true && cache.sets[set_idx].entries[i].tag == tag){
            cache_stats.num_hits++;
            ret.data = cache.sets[set_idx].entries[i].data[offset];
            ret.mem_fault = false;
            return ret;
        }
    }
    cache_stats.num_misses++;
    // if cache is full
    if(cache.sets[set_idx].entries_count == cache.sets[set_idx].size){
        // replace random entry
        uint8_t idx = rand() % cache.sets[set_idx].size;
        cache.sets[set_idx].entries[idx].valid = true;
        cache.sets[set_idx].entries[idx].tag = tag;
        cache.sets[set_idx].entries[idx].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
        ret.data = cache.sets[set_idx].entries[idx].data[offset];
        ret.mem_fault = true;
        return ret;
    }
    else{
        // add new entry
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].valid = true;
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].tag = tag;
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
        cache.sets[set_idx].entries_count++;
        ret.data = cache.sets[set_idx].entries[cache.sets[set_idx].entries_count - 1].data[offset];
        ret.mem_fault = true;
        return ret;
    }
}   

data_ret_t CACHE::get_data_fully_associative(uint32_t addr){
    uint8_t offset = addr & CACHE_OFFSET_MASK;
    uint32_t tag = addr >> (CACHE_OFFSET_SHIFT);

    data_ret_t ret;

    for(int i = 0; i < cache.sets[0].entries_count; i++){
        if(cache.sets[0].entries[i].valid == true && cache.sets[0].entries[i].tag == tag){
            cache_stats.num_hits++;
            ret.data = cache.sets[0].entries[i].data[offset];
            ret.mem_fault = false;
            return ret;
        }
    }
    cache_stats.num_misses++;
    // if cache is full
    if(cache.sets[0].entries_count == cache.sets[0].size){
        // replace random entry
        uint8_t idx = rand() % cache.sets[0].size;
        cache.sets[0].entries[idx].valid = true;
        cache.sets[0].entries[idx].tag = tag;
        cache.sets[0].entries[idx].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
        ret.data = cache.sets[0].entries[idx].data[offset];
        ret.mem_fault = true;
        return ret;
    }
    else{
        // add new entry
        cache.sets[0].entries[cache.sets[0].entries_count].valid = true;
        cache.sets[0].entries[cache.sets[0].entries_count].tag = tag;
        cache.sets[0].entries[cache.sets[0].entries_count].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
        cache.sets[0].entries_count++;
        ret.data = cache.sets[0].entries[cache.sets[0].entries_count - 1].data[offset];
        ret.mem_fault = true;
        return ret;
    }
}

data_ret_t CACHE::get_data(uint32_t addr){
    if(cache.type == DIRECT_MAPPED){
        return get_data_direct_mapped(addr);
    }
    else if(cache.type == FULLY_ASSOCIATIVE){
        return get_data_fully_associative(addr);
    }
    else{
        return get_data_set_associative(addr);
    }
}

data_ret_t CACHE::set_data_direct_mapped(uint32_t addr, uint32_t data){
    uint8_t offset = addr & CACHE_OFFSET_MASK;
    uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
    uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));
    
    data_ret_t  ret;

    if(cache.sets[set_idx].entries[0].valid == true && cache.sets[set_idx].entries[0].tag == tag){
        cache_stats.num_hits++;
        cache.sets[set_idx].entries[0].data[offset] = data;
        cache.sets[set_idx].entries[0].modified = true;
        ret.data = cache.sets[set_idx].entries[0].data[offset];
        ret.mem_fault = false;
        return ret;
    }
    else{
        cache_stats.num_misses++;
        // printf("[Cache] Miss %d\n",cache_stats.num_misses);
        cache.sets[set_idx].entries[0].valid = true;
        cache.sets[set_idx].entries[0].tag = tag;
        cache.sets[set_idx].entries[0].data[offset] = data;
        cache.sets[set_idx].entries[0].modified = true;
        ret.data = cache.sets[set_idx].entries[0].data[offset];
        ret.mem_fault = true;
        return ret;
    }
}

data_ret_t CACHE::set_data_fully_associative(uint32_t addr, uint32_t data){
    uint8_t offset = addr & CACHE_OFFSET_MASK;
    uint32_t tag = addr >> (CACHE_OFFSET_SHIFT);

    data_ret_t ret;

    for(int i = 0; i < cache.sets[0].entries_count; i++){
        if(cache.sets[0].entries[i].valid == true && cache.sets[0].entries[i].tag == tag){
            cache_stats.num_hits++;
            cache.sets[0].entries[i].data[offset] = data;
            cache.sets[0].entries[i].modified = true;
            ret.data = cache.sets[0].entries[i].data[offset];
            ret.mem_fault = false;
            return ret;
        }
    }
    cache_stats.num_misses++;
    // if cache is full
    if(cache.sets[0].entries_count == cache.sets[0].size){
        // replace random entry
        uint8_t idx = rand() % cache.sets[0].size;
        cache.sets[0].entries[idx].valid = true;
        cache.sets[0].entries[idx].tag = tag;
        cache.sets[0].entries[idx].data[offset] = data;
        cache.sets[0].entries[idx].modified = true;
        ret.data = cache.sets[0].entries[idx].data[offset];
        ret.mem_fault = true;
        return ret;
    }
    else{
        // add new entry
        cache.sets[0].entries[cache.sets[0].entries_count].valid = true;
        cache.sets[0].entries[cache.sets[0].entries_count].tag = tag;
        cache.sets[0].entries[cache.sets[0].entries_count].data[offset] = data;
        cache.sets[0].entries[cache.sets[0].entries_count].modified = true;
        cache.sets[0].entries_count++;
        ret.data = cache.sets[0].entries[cache.sets[0].entries_count - 1].data[offset];
        ret.mem_fault = true;
        return ret;
    }
}

data_ret_t CACHE::set_data_set_associative(uint32_t addr, uint32_t data){
    uint8_t offset = addr & CACHE_OFFSET_MASK;
    uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
    uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));
    
    data_ret_t ret;
    
    for(int i = 0; i < cache.sets[set_idx].entries_count; i++){
        if(cache.sets[set_idx].entries[i].valid == true && cache.sets[set_idx].entries[i].tag == tag){
            cache_stats.num_hits++;
            cache.sets[set_idx].entries[i].data[offset] = data;
            cache.sets[set_idx].entries[i].modified = true;
            ret.data = cache.sets[set_idx].entries[i].data[offset];
            ret.mem_fault = false;
            return ret;
        }
    }
    cache_stats.num_misses++;
    // if cache is full
    if(cache.sets[set_idx].entries_count == cache.sets[set_idx].size){
        // replace random entry
        uint8_t idx = rand() % cache.sets[set_idx].size;
        cache.sets[set_idx].entries[idx].valid = true;
        cache.sets[set_idx].entries[idx].tag = tag;
        cache.sets[set_idx].entries[idx].data[offset] = data;
        cache.sets[set_idx].entries[idx].modified = true;
        ret.data = cache.sets[set_idx].entries[idx].data[offset];
        ret.mem_fault = true;
        return ret;
    }
    else{
        // add new entry
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].valid = true;
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].tag = tag;
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].data[offset] = data;
        cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].modified = true;
        cache.sets[set_idx].entries_count++;
        ret.data = cache.sets[set_idx].entries[cache.sets[set_idx].entries_count - 1].data[offset];
        ret.mem_fault = true;
        return ret;
    }
}

data_ret_t CACHE::set_data(uint32_t addr, uint32_t data){
    if(cache.type == DIRECT_MAPPED){
        return set_data_direct_mapped(addr, data);
    }
    else if(cache.type == FULLY_ASSOCIATIVE){
        return set_data_fully_associative(addr, data);
    }
    else{
        return set_data_set_associative(addr, data);
    }

}

data_ret_t CACHE::exec(uint32_t addr, uint32_t data ,access_type_t access_type){
    if(access_type == READ){
        return get_data(addr);
    }
    else{
        return set_data(addr, 0);
    }
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
