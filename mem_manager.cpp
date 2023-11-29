#include "mem_manager.h"

MEM_MANAGER::MEM_MANAGER()
{
    vm = new VM();
    // L1 cache direct mapped 8 sets 1 entry replacement policy: none
    L1CacheInstr = new CACHE(CACHE::DIRECT_MAPPED, 512, 1, CACHE::FIFO);
    L1CacheData = new CACHE(CACHE::DIRECT_MAPPED, 512, 1, CACHE::NONE);
    // L2 cache fully associative 1 set 8 entries replacement policy: Random
    L2Cache = new CACHE(CACHE::SET_ASSOCIATIVE, 128, 8, CACHE::FIFO);
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