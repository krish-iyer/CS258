#ifndef TLB_H
#define TLB_H

#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>

class TLB{

    public:
        TLB(uint8_t tlb_size);
        uint32_t get_page(uint32_t virtual_addr);
        void add_page(uint32_t virtual_addr, uint32_t physical_addr);
        void print_stats();
        virtual ~TLB();
        
    private:
        typedef struct tlb_entry{
            // virtual address 29 bits
            uint32_t virtual_addr;
            // physical address 20 bits
            uint32_t physical_addr;
            bool valid;
        }tlb_entry_t;

        typedef struct tlb_stats
        {
            uint32_t num_hits;
            uint32_t num_misses;
            uint32_t num_entries;
        }tlb_stats_t;
        

    private:
        tlb_entry_t *tlb;
        uint32_t size;
    public:
        tlb_stats_t tlb_stats;
};


#endif // TLB_H