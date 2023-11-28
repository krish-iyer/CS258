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

#define VM_ADDR_MASK                0x000FFFFF
#define FIRST_LEVEL_VM_ADDR_MASK    0x000FC000
#define SECOND_LEVEL_VM_ADDR_MASK   0x00003FC0
#define THIRD_LEVEL_VM_ADDR_MASK    0x0000003F
#define OFFSET_MASK                 0x000001FF
#define FIRST_LEVEL_VM_ADDR_SHIFT   14
#define SECOND_LEVEL_VM_ADDR_SHIFT  6
#define OFFSET_SHIFT                9

#define FIRST_LEVEL_IDX(addr)  (((addr >> OFFSET_SHIFT) & FIRST_LEVEL_VM_ADDR_MASK)  >> FIRST_LEVEL_VM_ADDR_SHIFT)
#define SECOND_LEVEL_IDX(addr) (((addr >> OFFSET_SHIFT) & SECOND_LEVEL_VM_ADDR_MASK) >> SECOND_LEVEL_VM_ADDR_SHIFT)
#define THIRD_LEVEL_IDX(addr)  ((addr  >> OFFSET_SHIFT) & THIRD_LEVEL_VM_ADDR_MASK)

#define FIRST_LEVEL_PAGE_SIZE   64
#define SECOND_LEVEL_PAGE_SIZE  256
#define THIRD_LEVEL_PAGE_SIZE   64
#define PHYSICAL_ADDR_MASK      0x000FFFFF

#define TLB_SIZE 16

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