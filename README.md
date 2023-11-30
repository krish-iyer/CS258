# Cache Simulator

This project consists of a Page Table, TLB and Cache simulator. Memory Manager(src/mem_manager.cpp/h) manages memory accesses and operations. It intialises VM, L1 and L2 cache. VM(src/vm.cpp/h) manages page allocation and page table along with TLB. Once virtual to physical is translated, request goes to the L1 cache or L2 depending on the if it's a hit or miss. 

Here's L1 cache is a direct mapped cache which is inclusive with L2 Cache. L1 is also split into L1 Data and L1 Instruction. IF/ READ/ WRITE from the trace are all considered to the memory accesses and don't implement different different functionalities.

Actual physical address is byte addressable. A page fault in the TLB simply signifies that requested entry doesn't exist. A page fault in the page table signifies that page couldn't be allocated. Currently, there's no support for swapping out the page table. TLB eviction happens randomly.  

```
src
  ├── cache.cpp
  ├── cache.h
  ├── common.cpp
  ├── common.h
  ├── main.cpp
  ├── mem_manager.cpp
  ├── mem_manager.h
  ├── tlb.cpp
  ├── tlb.h
  ├── vm.cpp
  └── vm.h
scripts
  ├── case1.sh
  ├── case2.sh
  ├── case3.sh
  ├── generate_graphs_out1.py
  └── run_tests.sh

```

# Analysis
## Case 1 : TLB and Cache

### TLB

The average TLB miss rate (misses/total access) for 8 and 16 entry TLB turned out to be as following. This was expected because increase in TLB size resulted in less number of misses and hence lesser miss rate.

```
TLB Size   TLB Miss Rate(%)    
8          1.335585
16         0.361260
```

### Cache Line size

### L1 Cache

In this case tests were done for 64 and 128 bit the cache line and different size of L1 cache. Here are few observations. L1 Cache is configured as direct mapped cached for this and following tests. It's also split into Instruction and Data cache equally.

  1. As expected miss rate decreases as the cache size increases.
  2. There's no significant change from the change in the cache line.
  3. Though in few traces cache line made significant improvement. 
  4. Data chache miss rates are higher than instruction. This hints a about the access pattern of the data. 
     1. Instructions are well localised and data bit more random.
  5. Size of cache leads to better improvement in D cache than I cache due the very reason that the random access are better aided by larger cache size.


```
Cache Line  L1 Size     L1 I-cache Miss Rate    L1 D-cache Miss Rate                           
64          128         0.328534                2.232449
            256         0.244526                1.759026
            512         0.244526                1.759026
128         128         0.318272                2.192686
            256         0.236717                1.699903
            512         0.230363                1.566205

```                                 

### L2 Cache

The expected trend could be observed, as cache size increases miss rate tends to decrease. Here cache lines make huge difference and that hint's access to data is done on the near by addresses. There's no good explanation on why for 64 bit cache line and 512 sets, the miss rate is 91% on reason could be that most requests are handled by L1 and the few that got through were not accessed frequently because of the inclusivity of the L1.

```
Cache Line  L2 Cache Size   L2 Miss Rate         
64          512             91.153104
            1024            84.172529
            2048            73.829991
128         512             32.168770
            1024            10.823383
            2048            6.181888
```

## Case 2 : Replacement Policy

In this case we have done some tests with different replacement policies. Here are few key observations

  1. LRU better for lower cache line size and lower cache sets or enteries(Direct Mapped). This also makes sense because in the constraint resource environment it become critical to perform eviction carefully.
  2. Though the point discussed above quicky vanishes as size increases. Not only that it starts to perform worse than the other two.
  3. FIFO performs similar to RANDOM this tells that both stratergies are favoured. Also, this report lack to reflect on why LRU performs worse. 

```
Cache Line  L2 Cache Size   Replacement Policy          L2 Miss Rate(%)
64          512             FIFO                        45.002341
                            LRU                         42.179677
                            RANDOM                      45.457021
            1024            FIFO                        33.918968
                            LRU                         32.465497
                            RANDOM                      34.726185
            2048            FIFO                        20.462897
                            LRU                         21.910359
                            RANDOM                      20.581154
128         512             FIFO                         7.630865
                            LRU                         13.498306
                            RANDOM                       7.632400
            1024            FIFO                         5.887512
                            LRU                         12.176607
                            RANDOM                       5.887512
            2048            FIFO                         5.599380
                            LRU                         11.928463
                            RANDOM                       5.599380
```                
## Case 3 : Cache Types

Here we try to plot the contrast between Type of the cache and other parameters. 

  1. 4 way set associative performs better through out.
  2. Direct map seems to lack behind because the implemented code is not flexible enough to moderate the size i.e a direct mapped cache is of set 1 hence become smaller as compared to other cache types that's it reflects bit more improvement.

```
Cache Line  L2 Cache Size   Type                Entries  L2 Miss Rate(%)       
64          512             DIRECT_MAPPED       1        91.153104
                            SET_ASSOCIATIVE     2        65.435958
                                                4        45.457021
            1024            DIRECT_MAPPED       1        84.172529
                            SET_ASSOCIATIVE     2        56.081946
                                                4        34.726185
            2048            DIRECT_MAPPED       1        73.829991
                            SET_ASSOCIATIVE     2        41.992861
                                                4        20.581154
128         512             DIRECT_MAPPED       1        32.168770
                            SET_ASSOCIATIVE     2         8.566733
                                                4         7.632400
            1024            DIRECT_MAPPED       1        10.823383
                            SET_ASSOCIATIVE     2         5.995008
                                                4         5.887512
            2048            DIRECT_MAPPED       1         6.181888
                            SET_ASSOCIATIVE     2         5.599380
                                                4         5.599380
```                                                                                         
The experimental results are quite consistent accross cases. Miss rates are averages accross different trace results. Though during the agrregation of results it was observed that different traces performed significantly different but for now it was considered not as a factor. 