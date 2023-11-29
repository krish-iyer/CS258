#include <stdio.h>
#include <fstream>
#include "mem_manager.h"
#include "cache.h"
#include <iostream>
#include <sstream>

int main(){

    MEM_MANAGER mem_manager;

    std::ifstream infile;
    infile.open("Traces/Spec_Benchmark/008.espresso.din");

    uint64_t count = 0;
    try {
        while(!infile.eof()){
            count++;
            std::string line;
            std::getline(infile, line, '\n');

            std::string word;
            std::stringstream ss(line);
            uint32_t instr[3];
            for(int i = 0 ; i < 3 ; i++)
            {
                std::getline(ss, word, ' '); 
                instr[i] = std::stoul(word.c_str(), nullptr, 16);
            }
            if(instr[0]<2){
                mem_manager.exec(instr[1], CACHE::READ, false);
            }        
            else{
                mem_manager.exec(instr[1], CACHE::READ, true);
            }
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << count <<'\n';
    }
 
   
    printf("TLB hits: %d misses: %d\n", mem_manager.vm->tlb->tlb_stats.num_hits, mem_manager.vm->tlb->tlb_stats.num_misses);
    printf("L1 Instr Cache hits: %d misses: %d\n", mem_manager.L1CacheInstr->cache_stats.num_hits, mem_manager.L1CacheInstr->cache_stats.num_misses);
    printf("L1 Data Cache hits: %d misses: %d\n", mem_manager.L1CacheData->cache_stats.num_hits, mem_manager.L1CacheData->cache_stats.num_misses);
    printf("L2 Cache hits: %d misses: %d\n", mem_manager.L2Cache->cache_stats.num_hits, mem_manager.L2Cache->cache_stats.num_misses);
    // printf("TLB hits: %d misses: %d\n", vm.tlb->tlb_stats.num_hits, vm.tlb->tlb_stats.num_misses);
    return 0;
}   