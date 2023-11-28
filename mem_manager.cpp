#include "mem_manager.h"

MEM_MANAGER::MEM_MANAGER()
{
    vm = new VM();
    // L1 cache direct mapped 8 sets 1 entry replacement policy: none
    L1Cache = new CACHE(CACHE::DIRECT_MAPPED, 8, 1, CACHE::NONE);
    // L2 cache fully associative 1 set 8 entries replacement policy: Random
    L2Cache = new CACHE(CACHE::FULLY_ASSOCIATIVE, 1, 8, CACHE::RANDOM);
}

bool MEM_MANAGER::exec(uint32_t virtual_addr, CACHE::access_type_t access_type)
{
    addr_ret_t addr_ret = vm->exec(virtual_addr);
    if (addr_ret.page_fault)
    {
        // printf("Error: %d\n", addr_ret.physical_addr);
        return false;
    }
    else
    {
        // printf("[SUCCESS] TLB hit: %d TLB miss: %d\n", vm->tlb->tlb_stats.num_hits, vm->tlb->tlb_stats.num_misses);
        data_ret_t data_ret_l1 = L1Cache->exec(virtual_addr, 0, access_type);
        // if(data_ret_l1.mem_fault){
        //     data_ret_t data_ret_l2 = L2Cache->exec(virtual_addr, 0, access_type);
        //     if (data_ret_l2.mem_fault)
        //     {
        //         // printf("Error: %d\n", data_ret_l2.data);
        //         return false;
        //     }
        //     else
        //     {
        //         // printf("Success: %d\n", data_ret_l2.data);
        //         return true;
        //     }
        // }
        return true;
    }
}

MEM_MANAGER::~MEM_MANAGER()
{
    delete vm;
    delete L1Cache;
    delete L2Cache;
}