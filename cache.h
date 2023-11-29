#ifndef CACHE_H
#define CACHE_H

// Memeory access is 4 bytes addressable
// Cache line 64 bytes
// OFFSET: 4 bits   [3:0]
// SET: 3 bits      [6:4]   depends on cache type
// TAG: 20 bits     [28:7]  depends on cache type
// 0 - valid ; 1 - invalid
// 0 - not modified ; 1 - modified

#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include "common.h"


class CACHE{
    public:
        typedef enum{
            READ,
            WRITE
        }access_type_t;

        typedef enum{
            NONE,
            RANDOM,
            LRU,
            FIFO
        }cache_replacement_policy_t;
        
        typedef enum{
            DIRECT_MAPPED,
            FULLY_ASSOCIATIVE,
            SET_ASSOCIATIVE
        }cache_type_t;

    private:
        typedef struct cache_entry{
            uint32_t tag;
            uint32_t *data;
            bool     modified;
            bool     valid;
        }cache_entry_t;

        typedef struct cache_set{
            cache_entry_t *entries;
            uint16_t       entries_count;
            uint32_t       size;
        }cache_set_t;

        typedef struct cache{
            cache_set_t *sets;
            uint32_t     set_count;
            // fixed size for all sets
            uint32_t      entry_size;
            cache_type_t type;
            uint32_t      block_size;
            uint32_t      offset_mask;
        }cache_t;

    public:
        typedef struct cache_stats{
            uint32_t num_hits;
            uint32_t num_misses;
        }cache_stats_t;

    public:
        CACHE(cache_type_t type, uint16_t size, uint16_t entry_size, cache_replacement_policy_t cache_policy, uint32_t cache_block_size);
        data_ret_t exec (uint32_t addr, uint32_t data ,access_type_t access_type);
        void print_stats();
        virtual ~CACHE();

    private:
        data_ret_t get_data_direct_mapped(uint32_t addr);
        data_ret_t get_data_set_associative(uint32_t addr); 
        data_ret_t get_data_fully_associative(uint32_t addr);
        data_ret_t get_data(uint32_t addr);
        data_ret_t set_data_direct_mapped(uint32_t addr, uint32_t data);
        data_ret_t set_data_fully_associative(uint32_t addr, uint32_t data);
        data_ret_t set_data_set_associative(uint32_t addr, uint32_t data);
        data_ret_t set_data(uint32_t addr, uint32_t data);
    
    private:
        cache_t cache;
        cache_replacement_policy_t policy;
    public:
        cache_stats_t cache_stats;
};

#endif // CACHE_H