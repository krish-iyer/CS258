#include "vm.h"

VM::VM(uint32_t tlb_size){
    // set addr counter to 0
    addr_count = 0;
    // initialize page table
    // calloc initializes all values to 0 as opposed to malloc
    tlb = new TLB(tlb_size);
    page_table.level_count = 3;
    page_table.levels = (level_t *)calloc(sizeof(level_t) * page_table.level_count, sizeof(level_t));
    
    // intialise page table levels
    page_table.levels[0].size = FIRST_LEVEL_PAGE_SIZE; 
    page_table.levels[1].size = SECOND_LEVEL_PAGE_SIZE;
    page_table.levels[2].size = THIRD_LEVEL_PAGE_SIZE;

    page_table.levels[0].page = (page_entry_t *)calloc(sizeof(page_entry_t) * page_table.levels[0].size, sizeof(page_entry_t));
    page_table.levels[1].page = (page_entry_t *)calloc(sizeof(page_entry_t) * page_table.levels[0].size * page_table.levels[1].size, sizeof(page_entry_t));
    page_table.levels[2].page = (page_entry_t *)calloc(sizeof(page_entry_t) * page_table.levels[0].size * page_table.levels[1].size * page_table.levels[2].size, sizeof(page_entry_t));
    
    // printf("Page Table Initialized \n");
}

addr_ret_t VM::exec(uint32_t virtual_addr){
    uint8_t first_level_idx  = FIRST_LEVEL_IDX(virtual_addr);
    uint8_t second_level_idx = SECOND_LEVEL_IDX(virtual_addr);
    uint8_t third_level_idx  = THIRD_LEVEL_IDX(virtual_addr);
    uint32_t offset = virtual_addr & OFFSET_MASK;
    uint32_t page_addr = virtual_addr >> OFFSET_SHIFT;

    uint32_t frame_addr = 0;
    addr_ret_t ret;
    // printf("First level idx: %d Second level idx: %d Third level idx: %d\n", first_level_idx, second_level_idx, third_level_idx);
    // check if page is already allocated
    ret = tlb->get_page(page_addr);
    if(ret.page_fault == true){
        if(page_table.levels[2].page[page_table.levels[1].page[page_table.levels[0].page[first_level_idx].addr * page_table.levels[1].size + second_level_idx].addr * page_table.levels[2].size + third_level_idx].valid){
            frame_addr = page_table.levels[2].page[page_table.levels[1].page[page_table.levels[0].page[first_level_idx].addr * page_table.levels[1].size + second_level_idx].addr * page_table.levels[2].size + third_level_idx].addr;
            tlb->add_page(page_addr, frame_addr);
            ret.physical_addr = frame_addr << OFFSET_SHIFT | offset;
            ret.page_fault = false;
            return ret;
        }
        else{
            // printf("Page not allocated\n");
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
                                // printf("Address space full\n");
                                ret.physical_addr = 0;
                                ret.page_fault = true;
                                return ret;
                            }
                            // printf("Page allocated addr: %x page: %d idx : %d i: %d j: %d allocated addr: %x\n", virtual_addr, \
                            //     page_table.levels[2].page[ i * j * page_table.levels[2].size + third_level_idx].valid, \
                            //     (i * page_table.levels[1].size + second_level_idx) * j + third_level_idx, i, j, \
                            //     page_table.levels[2].page[j * page_table.levels[2].size + third_level_idx].addr);
                            frame_addr = page_table.levels[2].page[j * page_table.levels[2].size + third_level_idx].addr;
                            tlb->add_page(page_addr, frame_addr);
                            ret.physical_addr = frame_addr << OFFSET_SHIFT | offset;
                            ret.page_fault = false;
                            return ret;
                        }
                    }

                }
                else{
                    //printf("Second level is already allocated or full\n");
                    ret.physical_addr = 0;
                    ret.page_fault = true;
                }
            }
            printf("Error: Page Table is full\n");
        }
    }
    else{
        return ret;
    }
    ret.physical_addr = 0;
    ret.page_fault = true;
    return ret;
}
        
uint32_t VM::get_new_addr(){
    // printf("Addr count: %d\n", addr_count);
    addr_count = (addr_count + 1) & PHYSICAL_ADDR_MASK;
    if(addr_count == 0){
        printf("Address space full\n");
        return 0;
    }
    return addr_count;
}


VM::~VM(){
    free(page_table.levels[0].page);
    free(page_table.levels[1].page);
    free(page_table.levels[2].page);
    free(page_table.levels);
    delete tlb;
}