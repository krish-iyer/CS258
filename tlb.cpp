#include "tlb.h"

TLB::TLB(uint8_t tlb_size){
    size = tlb_size;
    tlb_stats.num_entries = 0;
    tlb_stats.num_misses = 0;
    tlb_stats.num_hits = 0;
    tlb = (tlb_entry_t *)calloc(sizeof(tlb_entry_t) * size, sizeof(tlb_entry_t));
}

uint32_t TLB::get_page(uint32_t virtual_addr){
    for(int i = 0; i < size; i++){
        if(tlb[i].virtual_addr == virtual_addr && tlb[i].valid == true){
            tlb_stats.num_hits++;
            return tlb[i].physical_addr;
        }
    }
    tlb_stats.num_misses++;
    return 0;
}

void TLB::add_page(uint32_t virtual_addr, uint32_t physical_addr){
    if(tlb_stats.num_entries < size){
        // add entry to tlb
        for(int i = 0; i < size; i++){
            if(tlb[i].valid == false){
                printf("[TLB] Adding entry to tlb virtual %x phy %x\n", virtual_addr, physical_addr);
                tlb[i].virtual_addr = virtual_addr;
                tlb[i].physical_addr = physical_addr;
                tlb[i].valid = true;
                tlb_stats.num_entries++;
                tlb_stats.num_misses++;
                return;
            }
        }
    }
    else{
        printf("[TLB] TLB is full entries %d size %d adding %x phy %x\n", virtual_addr, physical_addr, tlb_stats.num_entries, size);
        uint8_t idx = rand() % size;
        tlb[idx].virtual_addr = virtual_addr;
        tlb[idx].physical_addr = physical_addr;
        tlb[idx].valid = true;
        tlb_stats.num_misses++;
    }
    return;
}

void TLB::print_stats(){
    printf("[TLB] hits: %d misses: %d\n", tlb_stats.num_hits, tlb_stats.num_misses);
}
    
TLB::~TLB(){
    free(tlb);
}
