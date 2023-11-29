#include "mem_manager.h"

MEM_MANAGER::MEM_MANAGER(uint32_t tlb_size, uint32_t cache_line_size, uint32_t l1_cache_size , uint32_t l2_cache_size, uint8_t l2_cache_entries, \
    CACHE::cache_replacement_policy_t l2_cache_policy, CACHE::cache_type_t l2_cache_type)
{
    vm = new VM(tlb_size);
    // L1 cache direct mapped 8 sets 1 entry replacement policy: none
    L1CacheInstr = new CACHE(CACHE::DIRECT_MAPPED, l1_cache_size/2, 1, CACHE::FIFO, cache_line_size);
    L1CacheData = new CACHE(CACHE::DIRECT_MAPPED, l1_cache_size/2, 1, CACHE::NONE, cache_line_size);
    // L2 cache fully associative 1 set 8 entries replacement policy: Random
    L2Cache = new CACHE(l2_cache_type, l2_cache_size, l2_cache_entries, l2_cache_policy, cache_line_size);
}

bool MEM_MANAGER::exec(uint32_t virtual_addr, CACHE::access_type_t access_type, bool is_instr)
{
    addr_ret_t addr_ret = vm->exec(virtual_addr);
    if (addr_ret.page_fault)
    {
        printf("Page Fault Error: %d\n", addr_ret.physical_addr);
        return false;
    }
    else
    {

        // printf("[SUCCESS] TLB hit: %d TLB miss: %d\n", vm->tlb->tlb_stats.num_hits, vm->tlb->tlb_stats.num_misses);
        data_ret_t data_ret_l1;
        if(is_instr){
            data_ret_l1 = L1CacheInstr->exec(addr_ret.physical_addr, 0, access_type);
        }
        else{
            data_ret_l1 = L1CacheData->exec(addr_ret.physical_addr, 0, access_type);
        }
        if(data_ret_l1.mem_fault){
            //printf("Error: %d\n", data_ret_l1.data);
            data_ret_t data_ret_l2 = L2Cache->exec(addr_ret.physical_addr, 0, access_type);
            if (data_ret_l2.mem_fault)
            {
                // printf("Error: %d\n", data_ret_l2.data);
                return false;
            }
            else
            {
                // printf("Success: %d\n", data_ret_l2.data);
                return true;
            }
        }
        return true;
    }
}

MEM_MANAGER::~MEM_MANAGER()
{
    delete vm;
    delete L1CacheInstr;
    delete L1CacheData;
    delete L2Cache;
}