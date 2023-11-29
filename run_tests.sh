#!/bin/zsh

# Get the list of files in the current directory
files=(*Traces/Spec_Benchmark/*)
# tlb_size=8
# cache_line_size=64
# l1_cache_size=32
# l2_cache_size=256
l2_cache_entries=4
case=2

# Print each file name
for cache_replacement_policy in "RANDOM" "LRU" "FIFO";
do
    for tlb_size in 8 16;
    do
        for cache_line_size in 64 128;
        do
            for l1_cache_size in 128 256 512;
            do
                for l2_cache_size in 512 1024 2048;
                do
                    for file in "${files[@]}"; do
                        ./main.out $tlb_size $cache_line_size $l1_cache_size $l2_cache_size $l2_cache_entries $cache_replacement_policy $case $file 
                    done
                done
            done
        done
    done
done