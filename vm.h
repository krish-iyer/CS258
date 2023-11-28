#ifndef VM_H
#define VM_H

#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include "tlb.h"
#include "common.h"
// Each memory access is 4 bytes
// 3-level page table
// 1st level: 6 bits
// 2nd level: 8 bits
// 3rd level: 6 bits
// offset: 9 bits
// virtual address length: 29 bits
// physical address length: 20 bits
// 0 - valid ; 1 - invalid


class VM{
    private:
        typedef struct page_entry{
            bool        valid;
            uint32_t    addr;
        }page_entry_t;

        typedef struct level{
            page_entry_t *page;
            uint16_t     size;
        }level_t;

        typedef struct page_table{
            level_t *levels;
            uint8_t level_count;
        }page_table_t;
    public:
        VM();
        addr_ret_t exec(uint32_t virtual_addr);
        ~VM();
    private:
        uint32_t get_new_addr();

    public:
        TLB *tlb;
    private:
        page_table_t page_table;
        uint32_t  addr_count;
};

#endif // VM_H