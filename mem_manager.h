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
        MEM_MANAGER();
        bool exec(uint32_t virtual_addr, CACHE::access_type_t access_type, bool is_instr);
        ~MEM_MANAGER();

};





#endif // MEM_MANAGER_H