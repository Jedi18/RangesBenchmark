#!/usr/bin/env bash

#SBATCH -o benchmark.out
#SBATCH -t 04:00:00
#SBATCH -p buran
#SBATCH -N 1

srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name UNIQUE_REPLACE_IF_FOR --num_iterations 25
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name UNIQUE_REMOVE_IF_FOR --num_iterations 25
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name REVERSE_REPLACE_IF_FOR --num_iterations 25
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name REVERSE_REMOVE_IF_FOR --num_iterations 25 
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name FOR_EACH_TRANSFORM --num_iterations 50 
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name REPLACE_IF_FOR --num_iterations 50
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name REMOVE_IF_FOR --num_iterations 25
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name UNIQUE_FOR --num_iterations 25
srun /work/jedi18/src/RangesBenchmark/build/bin/main --algorithm_name REVERSE_FOR --num_iterations 50
