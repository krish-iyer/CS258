#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>

// Virtual Memory Stuff
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

// TLB Stuff
#define TLB_SIZE 16

// Cache Stuff
// Memeory access is 4 bytes addressable
// Cache line 64 bytes
// OFFSET: 4 bits   [3:0]
// SET: 3 bits      [6:4]   depends on cache type
// TAG: 20 bits     [28:7]  depends on cache type
// 0 - valid ; 1 - invalid

#define MEM_BIT_LEN         2 // 4 bytes : 1 << 2
#define CACHE_LINE_SIZE     64
#define CACHE_OFFSET_MASK   0x0000000F
#define CACHE_OFFSET_SHIFT  4

typedef enum{
    DIRECT_MAPPED,
    FULLY_ASSOCIATIVE,
    SET_ASSOCIATIVE
}cache_type_t;

typedef enum{
    READ,
    WRITE
}access_type_t;

typedef enum{
    RANDOM,
    LRU,
    LFU,
    FIFO
}cache_replacement_policy_t;

uint16_t count_bits(uint32_t num){
    uint16_t count = 0;
    while(num){
        count++;
        num >>= 1;
    }
    return count;
}

class CACHE{
    private:


        typedef struct cache_entry{
            uint32_t tag;
            uint32_t data[CACHE_LINE_SIZE >> MEM_BIT_LEN];
            bool     modified;
            bool     valid;
        }cache_entry_t;

        typedef struct cache_set{
            cache_entry_t *entries;
            uint8_t       entries_count;
            uint8_t       size;
        }cache_set_t;

        typedef struct cache{
            cache_set_t *sets;
            uint8_t     set_count;
            // fixed size for all sets
            uint8_t      entry_size;
            cache_type_t type;
        }cache_t;

        typedef struct cache_stats{
            uint32_t num_hits;
            uint32_t num_misses;
        }cache_stats_t;

    public:
        CACHE(cache_type_t type, uint8_t size, uint8_t entry_size, cache_replacement_policy_t cache_policy){
            // initialize cache
            policy = cache_policy;
            cache.set_count = size;
            cache.type = type;

            if(cache.type == DIRECT_MAPPED){
                cache.entry_size = 1; // for direct mapped cache
            }
            else if(cache.type == FULLY_ASSOCIATIVE){
                cache.entry_size = cache.set_count; // for fully associative cache
            }
            else if(cache.type == SET_ASSOCIATIVE){
                cache.entry_size = entry_size; // for set associative cache
            }
            // initialize cache sets
            cache.sets = (cache_set_t *)calloc(sizeof(cache_set_t) * cache.set_count, sizeof(cache_set_t));
            // initialize cache sets
            for(int i = 0; i < cache.set_count; i++){
                cache.sets[i].entries_count = 0;
                cache.sets[i].size = cache.entry_size;
                cache.sets[i].entries = (cache_entry_t *)calloc(sizeof(cache_entry_t) * cache.sets[i].size, sizeof(cache_entry_t));
            }
            // initialize cache stats
            cache_stats.num_hits = 0;
            cache_stats.num_misses = 0;
        }
        
        uint32_t get_data_direct_mapped(uint32_t addr){
            uint8_t offset = addr & CACHE_OFFSET_MASK;
            uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
            uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));
            
            if(cache.sets[set_idx].entries[0].valid == true && cache.sets[set_idx].entries[0].tag == tag){
                cache_stats.num_hits++;
                return cache.sets[set_idx].entries[0].data[offset];
            }
            else{
                cache_stats.num_misses++;
                cache.sets[set_idx].entries[0].valid = true;
                cache.sets[set_idx].entries[0].tag = tag;
                cache.sets[set_idx].entries[0].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
                return cache.sets[set_idx].entries[0].data[offset];
            }
        }

        uint32_t get_data_set_associative(uint32_t addr){
            uint8_t offset = addr & CACHE_OFFSET_MASK;
            uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
            uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));

            for(int i = 0; i < cache.sets[set_idx].entries_count; i++){
                if(cache.sets[set_idx].entries[i].valid == true && cache.sets[set_idx].entries[i].tag == tag){
                    cache_stats.num_hits++;
                    return cache.sets[set_idx].entries[i].data[offset];
                }
            }
            cache_stats.num_misses++;
            // if cache is full
            if(cache.sets[set_idx].entries_count == cache.sets[set_idx].size){
                // replace random entry
                uint8_t idx = rand() % cache.sets[set_idx].size;
                cache.sets[set_idx].entries[idx].valid = true;
                cache.sets[set_idx].entries[idx].tag = tag;
                cache.sets[set_idx].entries[idx].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
                return cache.sets[set_idx].entries[idx].data[offset];
            }
            else{
                // add new entry
                cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].valid = true;
                cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].tag = tag;
                cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
                cache.sets[set_idx].entries_count++;
                return cache.sets[set_idx].entries[cache.sets[set_idx].entries_count - 1].data[offset];
            }
        }   

        uint32_t get_data_fully_associative(uint32_t addr){
            uint8_t offset = addr & CACHE_OFFSET_MASK;
            uint32_t tag = addr >> (CACHE_OFFSET_SHIFT);

            for(int i = 0; i < cache.sets[0].entries_count; i++){
                if(cache.sets[0].entries[i].valid == true && cache.sets[0].entries[i].tag == tag){
                    cache_stats.num_hits++;
                    return cache.sets[0].entries[i].data[offset];
                }
            }
            cache_stats.num_misses++;
            // if cache is full
            if(cache.sets[0].entries_count == cache.sets[0].size){
                // replace random entry
                uint8_t idx = rand() % cache.sets[0].size;
                cache.sets[0].entries[idx].valid = true;
                cache.sets[0].entries[idx].tag = tag;
                cache.sets[0].entries[idx].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
                return cache.sets[0].entries[idx].data[offset];
            }
            else{
                // add new entry
                cache.sets[0].entries[cache.sets[0].entries_count].valid = true;
                cache.sets[0].entries[cache.sets[0].entries_count].tag = tag;
                cache.sets[0].entries[cache.sets[0].entries_count].data[offset] = 0; // TODO: data not implemented ; issue a read from memory
                cache.sets[0].entries_count++;
                return cache.sets[0].entries[cache.sets[0].entries_count - 1].data[offset];
            }
        }

        uint32_t get_data(uint32_t addr){
            if(cache.type == DIRECT_MAPPED){
                return get_data_direct_mapped(addr);
            }
            else if(cache.type == FULLY_ASSOCIATIVE){
                return get_data_fully_associative(addr);
            }
            else if(cache.type == SET_ASSOCIATIVE){
                return get_data_set_associative(addr);
            }
            return 0;
        }

        uint32_t set_data_direct_mapped(uint32_t addr, uint32_t data){
            uint8_t offset = addr & CACHE_OFFSET_MASK;
            uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
            uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));
            
            if(cache.sets[set_idx].entries[0].valid == true && cache.sets[set_idx].entries[0].tag == tag){
                cache_stats.num_hits++;
                cache.sets[set_idx].entries[0].data[offset] = data;
                cache.sets[set_idx].entries[0].modified = true;
                return cache.sets[set_idx].entries[0].data[offset];
            }
            else{
                cache_stats.num_misses++;
                cache.sets[set_idx].entries[0].valid = true;
                cache.sets[set_idx].entries[0].tag = tag;
                cache.sets[set_idx].entries[0].data[offset] = data;
                cache.sets[set_idx].entries[0].modified = true;
                return cache.sets[set_idx].entries[0].data[offset];
            }
        }

        uint32_t set_data_fully_associative(uint32_t addr, uint32_t data){
            uint8_t offset = addr & CACHE_OFFSET_MASK;
            uint32_t tag = addr >> (CACHE_OFFSET_SHIFT);

            for(int i = 0; i < cache.sets[0].entries_count; i++){
                if(cache.sets[0].entries[i].valid == true && cache.sets[0].entries[i].tag == tag){
                    cache_stats.num_hits++;
                    cache.sets[0].entries[i].data[offset] = data;
                    cache.sets[0].entries[i].modified = true;
                    return cache.sets[0].entries[i].data[offset];
                }
            }
            cache_stats.num_misses++;
            // if cache is full
            if(cache.sets[0].entries_count == cache.sets[0].size){
                // replace random entry
                uint8_t idx = rand() % cache.sets[0].size;
                cache.sets[0].entries[idx].valid = true;
                cache.sets[0].entries[idx].tag = tag;
                cache.sets[0].entries[idx].data[offset] = data;
                cache.sets[0].entries[idx].modified = true;
                return cache.sets[0].entries[idx].data[offset];
            }
            else{
                // add new entry
                cache.sets[0].entries[cache.sets[0].entries_count].valid = true;
                cache.sets[0].entries[cache.sets[0].entries_count].tag = tag;
                cache.sets[0].entries[cache.sets[0].entries_count].data[offset] = data;
                cache.sets[0].entries[cache.sets[0].entries_count].modified = true;
                cache.sets[0].entries_count++;
                return cache.sets[0].entries[cache.sets[0].entries_count - 1].data[offset];
            }
        }

        uint32_t set_data_set_associative(uint32_t addr, uint32_t data){
            uint8_t offset = addr & CACHE_OFFSET_MASK;
            uint8_t set_idx = (addr >> CACHE_OFFSET_SHIFT) & (cache.set_count - 1);
            uint32_t tag = addr >> (CACHE_OFFSET_SHIFT + count_bits(cache.set_count - 1));

            for(int i = 0; i < cache.sets[set_idx].entries_count; i++){
                if(cache.sets[set_idx].entries[i].valid == true && cache.sets[set_idx].entries[i].tag == tag){
                    cache_stats.num_hits++;
                    cache.sets[set_idx].entries[i].data[offset] = data;
                    cache.sets[set_idx].entries[i].modified = true;
                    return cache.sets[set_idx].entries[i].data[offset];
                }
            }
            cache_stats.num_misses++;
            // if cache is full
            if(cache.sets[set_idx].entries_count == cache.sets[set_idx].size){
                // replace random entry
                uint8_t idx = rand() % cache.sets[set_idx].size;
                cache.sets[set_idx].entries[idx].valid = true;
                cache.sets[set_idx].entries[idx].tag = tag;
                cache.sets[set_idx].entries[idx].data[offset] = data;
                cache.sets[set_idx].entries[idx].modified = true;
                return cache.sets[set_idx].entries[idx].data[offset];
            }
            else{
                // add new entry
                cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].valid = true;
                cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].tag = tag;
                cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].data[offset] = data;
                cache.sets[set_idx].entries[cache.sets[set_idx].entries_count].modified = true;
                cache.sets[set_idx].entries_count++;
                return cache.sets[set_idx].entries[cache.sets[set_idx].entries_count - 1].data[offset];
            }
        }

        uint32_t set_data(uint32_t addr, uint32_t data){
            if(cache.type == DIRECT_MAPPED){
                return set_data_direct_mapped(addr, data);
            }
            else if(cache.type == FULLY_ASSOCIATIVE){
                return set_data_fully_associative(addr, data);
            }
            else if(cache.type == SET_ASSOCIATIVE){
                return set_data_set_associative(addr, data);
            }
            return 0;
        }

        uint32_t exec (uint32_t addr, uint32_t data ,access_type_t access_type){
            if(access_type == READ){
                return get_data(addr);
            }
            else if(access_type == WRITE){
                return set_data(addr, 0);
            }
        }

        void print_stats(){
            printf("[CACHE] hits: %d misses: %d\n", cache_stats.num_hits, cache_stats.num_misses);
        }

        virtual ~CACHE(){
            for(int i = 0; i < cache.set_count; i++){
                free(cache.sets[i].entries);
            }
            free(cache.sets);
        }
    private:
        cache_t cache;
        cache_stats_t cache_stats;
        cache_replacement_policy_t policy;
        
};


class TLB{

    public:
        TLB(uint8_t tlb_size){
            size = tlb_size;
            tlb_stats.num_entries = 0;
            tlb_stats.num_misses = 0;
            tlb_stats.num_hits = 0;
            tlb = (tlb_entry_t *)calloc(sizeof(tlb_entry_t) * size, sizeof(tlb_entry_t));
        }
        
        uint32_t get_page(uint32_t virtual_addr){
            for(int i = 0; i < size; i++){
                if(tlb[i].virtual_addr == virtual_addr && tlb[i].valid == true){
                    tlb_stats.num_hits++;
                    return tlb[i].physical_addr;
                }
            }
            tlb_stats.num_misses++;
            return 0;
        }

        void add_page(uint32_t virtual_addr, uint32_t physical_addr){
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

        void print_stats(){
            printf("[TLB] hits: %d misses: %d\n", tlb_stats.num_hits, tlb_stats.num_misses);
        }
    
        virtual ~TLB(){
            free(tlb);
        }
        
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
        VM(){
            // set addr counter to 0
            addr_count = 0;
            // initialize page table
            // calloc initializes all values to 0 as opposed to malloc
            tlb = new TLB(TLB_SIZE);
            page_table.level_count = 3;
            page_table.levels = (level_t *)calloc(sizeof(level_t) * page_table.level_count, sizeof(level_t));
            
            // intialise page table levels
            page_table.levels[0].size = FIRST_LEVEL_PAGE_SIZE; 
            page_table.levels[1].size = SECOND_LEVEL_PAGE_SIZE;
            page_table.levels[2].size = THIRD_LEVEL_PAGE_SIZE;

            page_table.levels[0].page = (page_entry_t *)calloc(sizeof(page_entry_t) * page_table.levels[0].size, sizeof(page_entry_t));
            page_table.levels[1].page = (page_entry_t *)calloc(sizeof(page_entry_t) * page_table.levels[0].size * page_table.levels[1].size, sizeof(page_entry_t));
            page_table.levels[2].page = (page_entry_t *)calloc(sizeof(page_entry_t) * page_table.levels[0].size * page_table.levels[1].size * page_table.levels[2].size, sizeof(page_entry_t));
            
            printf("Page Table Initialized \n");
        }


        //  ret:
        //  0 - page already allocated
        //  1 - page allocated
        // -1 - invalid address

        int8_t alloc_page(uint32_t virtual_addr){
            uint8_t first_level_idx  = FIRST_LEVEL_IDX(virtual_addr);
            uint8_t second_level_idx = SECOND_LEVEL_IDX(virtual_addr);
            uint8_t third_level_idx  = THIRD_LEVEL_IDX(virtual_addr);

            printf("First level idx: %d Second level idx: %d Third level idx: %d\n", first_level_idx, second_level_idx, third_level_idx);
            // check if page is already allocated
            if(is_valid_addr(virtual_addr)){
                printf("Page already allocated %x\n",virtual_addr);
                return 0;
            }

            // allocate new page
            // check if first level page is allocated
            for(int i = 1; i <= page_table.levels[0].size; i++){
                // find an empty page in second level
                if(page_table.levels[1].page[i * page_table.levels[1].size + second_level_idx].valid == false){
                    // find an empty page in third level
                    for(int j = 1; j <= page_table.levels[1].size; j++){
                        if( page_table.levels[2].page[(i * page_table.levels[1].size + second_level_idx) * j + third_level_idx].valid == false){
                            // allocate page
                            page_table.levels[0].page[first_level_idx].valid = true;
                            page_table.levels[1].page[i * page_table.levels[1].size + second_level_idx].valid = true;
                            page_table.levels[2].page[j * page_table.levels[2].size + third_level_idx].valid = true;
                            page_table.levels[0].page[first_level_idx].addr = i;
                            page_table.levels[1].page[i * page_table.levels[1].size + second_level_idx].addr = j;
                            page_table.levels[2].page[j * page_table.levels[2].size + third_level_idx].addr = get_new_addr();

                            if(page_table.levels[2].page[j * page_table.levels[2].size + third_level_idx].addr == 0){
                                printf("Address space full\n");
                                return -2;
                            }
                            
                            printf("Page allocated addr: %x page: %d idx : %d i: %d j: %d allocated addr: %x\n", virtual_addr, \
                                page_table.levels[2].page[ i * j * page_table.levels[2].size + third_level_idx].valid, \
                                (i * page_table.levels[1].size + second_level_idx) * j + third_level_idx, i, j, \
                                page_table.levels[2].page[j * page_table.levels[2].size + third_level_idx].addr);
                            tlb->add_page(virtual_addr, page_table.levels[2].page[j * page_table.levels[2].size + third_level_idx].addr);
                            return 1;
                        }
                        else{
                            printf("Third level is already allocated or full addr: %x page: %d idx : %d i: %d j: %d\n", virtual_addr, \
                                page_table.levels[2].page[ i * j * page_table.levels[2].size + third_level_idx].valid, \
                                (i * page_table.levels[1].size + second_level_idx) * j + third_level_idx, i, j);
                            //return -1;
                        }
                    }

                }
                else{
                    printf("Second level is already allocated or full\n");
                    return -1;
                }
            }
            printf("Error\n");
            return -2;
        }

        uint32_t get_page(uint32_t virtual_addr){
            uint8_t first_level_idx  = FIRST_LEVEL_IDX(virtual_addr);
            uint8_t second_level_idx = SECOND_LEVEL_IDX(virtual_addr);
            uint8_t third_level_idx  = THIRD_LEVEL_IDX(virtual_addr);

            uint32_t physical_addr = 0;
            //printf("First level idx: %d Second level idx: %d Third level idx: %d\n", first_level_idx, second_level_idx, third_level_idx);
            //printf("Reading address idx: %d \n", page_table.levels[1].page[page_table.levels[0].page[first_level_idx].addr * page_table.levels[1].size + second_level_idx].addr * page_table.levels[2].size + third_level_idx);
            //printf("translated addr : %x\n",page_table.levels[2].page[page_table.levels[1].page[page_table.levels[0].page[first_level_idx].addr * page_table.levels[1].size + second_level_idx].addr * page_table.levels[2].size + third_level_idx].addr);
            // check if page is already allocated
            if(is_valid_addr(virtual_addr)){
                printf("Returning valid page\n");
                physical_addr = tlb->get_page(virtual_addr);
                if(physical_addr == 0){
                    physical_addr = page_table.levels[2].page[page_table.levels[1].page[page_table.levels[0].page[first_level_idx].addr * page_table.levels[1].size + second_level_idx].addr * page_table.levels[2].size + third_level_idx].addr;
                    tlb->add_page(virtual_addr, physical_addr);
                }
                return physical_addr;
            }
            return 0;
        }

        ~VM(){
            free(page_table.levels[0].page);
            free(page_table.levels[1].page);
            free(page_table.levels[2].page);
            free(page_table.levels);
            delete tlb;
        }
        
    private:
        uint32_t get_new_addr(){
            printf("Addr count: %d\n", addr_count);
            addr_count = (addr_count + 1) & PHYSICAL_ADDR_MASK;
            if(addr_count == 0){
                printf("Address space full\n");
                return 0;
            }
            return addr_count;
        }

        bool is_valid_addr(uint32_t virtual_addr){
            uint8_t first_level_idx  = FIRST_LEVEL_IDX(virtual_addr);
            uint8_t second_level_idx = SECOND_LEVEL_IDX(virtual_addr);
            uint8_t third_level_idx  = THIRD_LEVEL_IDX(virtual_addr);

            uint8_t second_level_page_idx = page_table.levels[0].page[first_level_idx].addr * page_table.levels[1].size + second_level_idx;
            uint8_t third_level_page_idx  = page_table.levels[1].page[second_level_page_idx].addr * page_table.levels[2].size + third_level_idx;
            // check if page is already allocated
            if(page_table.levels[2].page[page_table.levels[1].page[page_table.levels[0].page[first_level_idx].addr * page_table.levels[1].size + second_level_idx].addr * page_table.levels[2].size + third_level_idx].valid){
                    return true;

            }
            return false;
        }

    public:
        TLB *tlb;
    private:
        page_table_t page_table;
        uint32_t  addr_count;
};


int main(){

    VM vm;
    uint32_t virtual_addr = 0x00DF0000;
    uint8_t ret = vm.alloc_page(virtual_addr);
    // virtual_addr = 0x00DF0000;
    // ret = vm.alloc_page(virtual_addr);
    virtual_addr = 0x00DC0000;
    ret = vm.alloc_page(virtual_addr);
    ret = vm.alloc_page(virtual_addr);
    ret = vm.alloc_page(virtual_addr);

    printf("Get page: %x addr: %x\n", virtual_addr, vm.get_page(virtual_addr));
    printf("Get page: %x addr: %x\n", virtual_addr, vm.get_page(virtual_addr));
    printf("TLB hits: %d misses: %d\n", vm.tlb->tlb_stats.num_hits, vm.tlb->tlb_stats.num_misses);
    return 0;
}   