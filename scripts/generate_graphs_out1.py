import pandas as pd

df = pd.read_csv('output_3.csv')

tlb_size_grp = df.groupby(['cache_replacement_policy', 'cache_line_size', 'l2_cache_size','l2_cache_type', 'l2_cache_entries', ])

res = tlb_size_grp[['l2_cache_miss_rate']].mean()*100


# tlb_size_grp = df.groupby(['l2_cache_size','cache_line_size','trace_file_name'])

# res = tlb_size_grp[['l2_cache_misses','l2_cache_hits']].mean()*100


#res.to_csv("output_1_processed.csv")

print(res)