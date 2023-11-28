#include "vm.h"

VM::VM(){
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

int8_t VM::alloc_page(uint32_t virtual_addr){
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

uint32_t VM::get_page(uint32_t virtual_addr){
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

VM::~VM(){
    free(page_table.levels[0].page);
    free(page_table.levels[1].page);
    free(page_table.levels[2].page);
    free(page_table.levels);
    delete tlb;
}
        
uint32_t VM::get_new_addr(){
    printf("Addr count: %d\n", addr_count);
    addr_count = (addr_count + 1) & PHYSICAL_ADDR_MASK;
    if(addr_count == 0){
        printf("Address space full\n");
        return 0;
    }
    return addr_count;
}

bool VM::is_valid_addr(uint32_t virtual_addr){
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
