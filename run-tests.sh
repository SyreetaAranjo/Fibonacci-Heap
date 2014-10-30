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

# Delete executable
rm fib-heaps;