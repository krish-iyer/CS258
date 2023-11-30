#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include "vm.h"
#include "cache.h"

class MEM_MANAGER
{
    public:
        VM *vm;
        CACHE *L1CacheInstr;
        CACHE *L1CacheData;
        CACHE *L2Cache;
        MEM_MANAGER(uint32_t tlb_size, uint32_t cache_line_size,  uint32_t l1_cache_size , \
            uint32_t l2_cache_size, uint8_t l2_cache_entries, \
            CACHE::cache_replacement_policy_t l2_cache_policy, CACHE::cache_type_t l2_cache_type);
        bool exec(uint32_t virtual_addr, CACHE::access_type_t access_type, bool is_instr);
        ~MEM_MANAGER();

};





#endif // MEM_MANAGER_H