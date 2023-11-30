#!/bin/zsh

cleanup() {
    for pid in ${pids[@]}; do
        kill $pid
    done
}
pids=()

trap cleanup EXIT

# Get the list of files in the current directory
files=(*Traces/Spec_Benchmark/*)
# tlb_size=8
# cache_line_size=64
# l1_cache_size=32
# l2_cache_size=256
l2_cache_entries=4
l2_cache_type="SET_ASSOCIATIVE"
case=2

echo -n "case,tlb_size,cache_line_size,l1_cache_size,l2_cache_size,l2_cache_entries,l2_cache_type,cache_replacement_policy,trace_file_name," > output_2.csv
echo -n "tlb_hits,tlb_misses,l1_icache_hits,l1_icache_misses,l1_dcache_hits,l1_dcache_misses,l2_cache_hits,l2_cache_misses," >> output_2.csv
echo -n "tlb_miss_rate,l1_icache_miss_rate,l1_dcache_miss_rate,l2_cache_miss_rate" >> output_2.csv
echo >> output_2.csv

# Print each file name
for file in "${files[@]}"; do
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
                        ./case2.out $tlb_size $cache_line_size $l1_cache_size $l2_cache_size \
                            $l2_cache_entries $l2_cache_type $cache_replacement_policy $case $file &
                        pids+=($!)
                    done
                done
            done
            wait
        done
    done
done