#ifndef COMMON_H
#define COMMON_H

#include<inttypes.h>

uint16_t count_bits(uint32_t num);
typedef struct addr_ret{
    uint32_t physical_addr;
    bool     page_fault;
}addr_ret_t;
#endif // COMMON_H