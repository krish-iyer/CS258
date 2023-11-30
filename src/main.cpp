#include <stdio.h>
#include <fstream>
#include "mem_manager.h"
#include "cache.h"
#include <iostream>
#include <sstream>


int main(int argc, char **argv){

    if(argc != 10){
        printf("Usage: ./mem_manager <tlb_size> <cache_line_size> <l1_cache_size> <l2_cache_size> <l2_cache_entries> <l2_cache_type> <cache_replacement_policy> <case> <trace_file>\n");
        return 0;
    }

    uint32_t tlb_size = std::stoul(argv[1]); 
    uint32_t cache_line_size = std::stoul(argv[2]);
    uint32_t l1_cache_size = std::stoul(argv[3]);
    uint32_t l2_cache_size = std::stoul(argv[4]);
    uint32_t l2_cache_entries = std::stoul(argv[5]);
    CACHE::cache_type_t l2_cache_type;

    if(std::string(argv[6]) == "DIRECT_MAPPED"){
        l2_cache_type = CACHE::DIRECT_MAPPED;
        l2_cache_entries = 1;
    }
    else if(std::string(argv[6]) == "SET_ASSOCIATIVE"){
        l2_cache_type = CACHE::SET_ASSOCIATIVE;
    }
    else{
        l2_cache_type = CACHE::FULLY_ASSOCIATIVE;
    }

    CACHE::cache_replacement_policy_t policy;

    if(std::string(argv[7]) == "RANDOM")
        policy = CACHE::RANDOM;
    else if(std::string(argv[7]) == "LRU")
        policy = CACHE::LRU;
    else if(std::string(argv[7]) == "FIFO")
        policy = CACHE::FIFO;
    else
        policy = CACHE::NONE;

    printf("L2 cache type %s policy : %s \n", argv[6], argv[7]);
    MEM_MANAGER mem_manager(tlb_size, cache_line_size, l1_cache_size, l2_cache_size, l2_cache_entries, policy, l2_cache_type);

    std::ifstream infile;
    std::ofstream outfile;

    outfile.open("output_"+std::string(argv[8])+".csv", std::ios::app | std::ios::out);

    infile.open(argv[9]);

    uint64_t count = 0;

    try {
        for (std::string line; std::getline(infile, line);) {
            count++;
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
    
    outfile << argv[8] << "," 
            << tlb_size << ","
            << cache_line_size << ","
            << l1_cache_size << ","
            << l2_cache_size << ","
            << l2_cache_entries << ","
            << argv[6] << ","
            << argv[7] << ","
            << argv[9] << ","
            << mem_manager.vm->tlb->tlb_stats.num_hits << ","
            << mem_manager.vm->tlb->tlb_stats.num_misses << ","
            << mem_manager.L1CacheInstr->cache_stats.num_hits << ","
            << mem_manager.L1CacheInstr->cache_stats.num_misses << ","
            << mem_manager.L1CacheData->cache_stats.num_hits << ","
            << mem_manager.L1CacheData->cache_stats.num_misses << ","
            << mem_manager.L2Cache->cache_stats.num_hits << ","
            << mem_manager.L2Cache->cache_stats.num_misses << ","
            << double(mem_manager.vm->tlb->tlb_stats.num_misses) / (mem_manager.vm->tlb->tlb_stats.num_misses + mem_manager.vm->tlb->tlb_stats.num_hits) << ","
            << double(mem_manager.L1CacheInstr->cache_stats.num_misses) / (mem_manager.L1CacheInstr->cache_stats.num_misses + mem_manager.L1CacheInstr->cache_stats.num_hits) << ","           
            << double(mem_manager.L1CacheData->cache_stats.num_misses) / (mem_manager.L1CacheData->cache_stats.num_misses + mem_manager.L1CacheData->cache_stats.num_hits) << ","
            << double(mem_manager.L2Cache->cache_stats.num_misses) / (mem_manager.L2Cache->cache_stats.num_misses + mem_manager.L2Cache->cache_stats.num_hits)
            << std::endl;
    
    outfile.close();
    return 0;
}   