#! /bin/bash

# Compile
g++ -Wall -o fib-heaps src/fib-heaps.cpp;

# Run tests
#for i in $(ls -d tests/*/); do 
#	echo ${i};
#	./fib-heaps < ${i}/input > out;
#	diff ${i}/output out;
#	rm out;
#done

# Benchmark
for i in $(ls benchmark/*); do 
	echo ${i};
	time ./fib-heaps < ${i} > /dev/null;
done

# Delete executable
rm fib-heaps;
