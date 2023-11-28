#ifndef COMMON_H
#define COMMON_H

#include<inttypes.h>

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

// structs
typedef struct addr_ret{
    uint32_t physical_addr;
    bool     page_fault;
}addr_ret_t;

// func
uint16_t count_bits(uint32_t num);

#endif // COMMON_H