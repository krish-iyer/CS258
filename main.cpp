#include <stdio.h>
#include "mem_manager.h"
#include "cache.h"

int main(){

    MEM_MANAGER mem_manager;
    uint32_t virtual_addr = 0x00DF0000;
    bool ret = mem_manager.exec(virtual_addr, CACHE::READ);
    // virtual_addr = 0x00DF0000;
    // ret = vm.alloc_page(virtual_addr);
    virtual_addr = 0x00DC0000;
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
    virtual_addr = 0x00DF0000;
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
     ret = mem_manager.exec(virtual_addr, CACHE::READ);
    printf("TLB hits: %d misses: %d\n", mem_manager.vm->tlb->tlb_stats.num_hits, mem_manager.vm->tlb->tlb_stats.num_misses);
    printf("L1 Cache hits: %d misses: %d\n", mem_manager.L1Cache->cache_stats.num_hits, mem_manager.L1Cache->cache_stats.num_misses);
    printf("L2 Cache hits: %d misses: %d\n", mem_manager.L2Cache->cache_stats.num_hits, mem_manager.L2Cache->cache_stats.num_misses);
    // printf("TLB hits: %d misses: %d\n", vm.tlb->tlb_stats.num_hits, vm.tlb->tlb_stats.num_misses);
    return 0;
}   