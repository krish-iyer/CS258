#include <stdio.h>
#include "vm.h"

int main(){

    VM vm;
    uint32_t virtual_addr = 0x00DF0000;
    uint8_t ret = vm.exec(virtual_addr);
    // virtual_addr = 0x00DF0000;
    // ret = vm.alloc_page(virtual_addr);
    virtual_addr = 0x00DC0000;
    ret = vm.exec(virtual_addr);
    ret = vm.exec(virtual_addr);
    ret = vm.exec(virtual_addr);

    virtual_addr = 0x00DF0000;

    ret = vm.exec(virtual_addr);
    ret = vm.exec(virtual_addr);

    printf("Get page: %x addr: %x\n", virtual_addr, vm.exec(virtual_addr));
    virtual_addr = 0x00DC0000;
    
    printf("Get page: %x addr: %x\n", virtual_addr, vm.exec(virtual_addr));
    printf("TLB hits: %d misses: %d\n", vm.tlb->tlb_stats.num_hits, vm.tlb->tlb_stats.num_misses);
    return 0;
}   