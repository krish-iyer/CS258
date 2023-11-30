#include "tlb.h"

TLB::TLB(uint8_t tlb_size){
    size = tlb_size;
    tlb_stats.num_entries = 0;
    tlb_stats.num_misses = 0;
    tlb_stats.num_hits = 0;
    tlb = (tlb_entry_t *)calloc(sizeof(tlb_entry_t) * size, sizeof(tlb_entry_t));
}

addr_ret_t TLB::get_page(uint32_t page_addr){
    addr_ret_t ret;
    for(int i = 0; i < size; i++){
        if(tlb[i].virtual_addr == page_addr && tlb[i].valid == true){
            tlb_stats.num_hits++;
            ret.physical_addr = tlb[i].physical_addr;
            ret.page_fault = false;
            return ret;
        }
    }
    ret.physical_addr = 0;
    ret.page_fault = true;
    tlb_stats.num_misses++;
    // print_stats();
    return ret;
}

void TLB::add_page(uint32_t page_addr, uint32_t frame_addr){
    // happened on a miss so no need to increment num_misses
    if(tlb_stats.num_entries < size){
        // add entry to tlb
        for(int i = 0; i < size; i++){
            if(tlb[i].valid == false){
                // printf("[TLB] Adding entry to tlb virtual %x phy %x\n", virtual_addr, physical_addr);
                tlb[i].virtual_addr = page_addr;
                tlb[i].physical_addr = frame_addr;
                tlb[i].valid = true;
                tlb_stats.num_entries++;
                return;
            }
        }
    }
    else{
        // printf("[TLB] TLB is full entries %d size %d adding %x phy %x\n", virtual_addr, physical_addr, tlb_stats.num_entries, size);
        // evict
        uint8_t idx = rand() % size;
        tlb[idx].virtual_addr = page_addr;
        tlb[idx].physical_addr = frame_addr;
        tlb[idx].valid = true;
    }
    //print_stats();
    return;
}

void TLB::print_stats(){
    printf("[TLB] hits: %d misses: %d\n", tlb_stats.num_hits, tlb_stats.num_misses);
}
    
TLB::~TLB(){
    free(tlb);
}