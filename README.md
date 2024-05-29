# 1️⃣🐝🏎️ The One Billion Row Challenge in C
I want to provide some benchmarks with C code and challenge implementations in other languages.

## Build the binary
gcc -O3 -o 1brc 1brc.c -lpthread

## Run the challenge

## Results
I am limited by the resources of my laptop which is Intel 4-core i7-9750H CPU @ 2.60GHz/ with 6G RAM. So I can only test with 100 million rows.
The first version of my code includes already some basic optimizations and gives an execution of 3.6s. I did further optimizations with multi-threads
on hashing and parsing. The newest version gives an execution time of 0.4s.

For comparaison, I took the top 3 java best-performers and another C implementation.

For anyone who is interested in running the code with 1 billion rows, please contact me.
