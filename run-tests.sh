#! /bin/bash

# Compile
clang++ -Wall -o fib-heaps src/fib-heaps.cpp;

# Run tests
for i in $(ls -d tests/*/); do 
	echo ${i};
	./fib-heaps < ${i}/input > out;
	diff ${i}/output out;
	rm out;
done

# Measure performance
for i in $(ls -d tests/*/); do 
	echo ${i};
	time ./fib-heaps < ${i}/input > /dev/null;
done

# Delete executable
rm fib-heaps;