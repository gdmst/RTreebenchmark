# run batch parameter 
#  1. operation type: 1 = insertion, 2 = deletion, 3 = point query, 4 = range query
#  2. index variant: 0 = linear, 1 = quardatic, 2 = star
#  3. query size (for range query: 10, 25, 50, 100)

#!/bin/bash
if [ $1 -eq 0 ]; then
    echo "range query Linear"
elif [ $1 -eq 1 ]; then
    echo "range query Quadratic"
elif [ $1 -eq 2 ]; then
    echo "range query Star"
fi

echo "longitude min, lattitude min, timestamp min, longitude min, lattitude min, timestamp min, query_size, found, elasped time"

index_variant=$1
query_size=$2

export DYLD_LIBRARY_PATH=/Users/uraiwanjansong/Research/Spatiotemporal_index/lib:$DYLD_LIBRARY_PATH

./out/benchmark 4 $index_variant $query_size



#  ./benchmark_testcase/run_benchmark_range_query.sh 0 10 >> results/test_range_query_10.csv