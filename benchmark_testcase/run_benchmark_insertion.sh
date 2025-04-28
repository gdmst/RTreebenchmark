# run batch parameter 
#  1. operation type: 1 = insertion, 2 = deletion, 3 = point query, 4 = range query
#  2. index variant: 0 = linear, 1 = quardatic, 2 = star
#  3. chunk size (for insertion, deletion, point query), or query size (for range query: 10, 25, 50, 100)

#!/bin/bash
if [ $1 -eq 0 ]; then
    echo "insert Linear"
elif [ $1 -eq 1 ]; then
    echo "insert Quadratic"
elif [ $1 -eq 2 ]; then
    echo "insert Star"
fi

echo "chunk size, average elapsed time"

index_variant=$1

while IFS= read -r line
do
  export DYLD_LIBRARY_PATH=/Users/uraiwanjansong/Research/Spatiotemporal_index/lib:$DYLD_LIBRARY_PATH
  chunk_size=$line

  ./out/benchmark 1 $index_variant $chunk_size
done < benchmark_testcase/chunk_size.txt


# ./benchmark_testcase/run_benchmark_insertion.sh 0 > results/test_insertion_linear.csv