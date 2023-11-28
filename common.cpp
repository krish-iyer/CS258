#include "common.h"

uint16_t count_bits(uint32_t num){
    uint16_t count = 0;
    while(num){
        count++;
        num >>= 1;
    }
    return count;
}