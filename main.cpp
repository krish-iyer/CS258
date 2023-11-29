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

    outfile.open("output_"+std::string(argv[8])+".txt", std::ios::app | std::ios::out);

    infile.open(argv[9]);

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
    
    outfile << "Case: " << argv[8] << " TLB size: " << tlb_size << " Cache line size: " << cache_line_size << " L1 Cache size: " << l1_cache_size << " L2 Cache size: " << l2_cache_size << " L2 Cache entries: " << l2_cache_entries << " L2 Cache Type: " << argv[6] << " Cache replacement policy: " << argv[7] \
        << " File: " << argv[9] << " TLB hits: " << mem_manager.vm->tlb->tlb_stats.num_hits << " misses: " << mem_manager.vm->tlb->tlb_stats.num_misses \
        << " L1 Instr Cache hits: " << mem_manager.L1CacheInstr->cache_stats.num_hits << " misses: " << mem_manager.L1CacheInstr->cache_stats.num_misses \
        << " L1 Data Cache hits: " << mem_manager.L1CacheData->cache_stats.num_hits << " misses: " << mem_manager.L1CacheData->cache_stats.num_misses \
        << " L2 Cache hits: " << mem_manager.L2Cache->cache_stats.num_hits << " misses: " << mem_manager.L2Cache->cache_stats.num_misses \
        << " L2 Cache hits: " << mem_manager.L2Cache->cache_stats.num_hits << " misses: " << mem_manager.L2Cache->cache_stats.num_misses << std::endl;
    
    outfile.close();
    return 0;
}   