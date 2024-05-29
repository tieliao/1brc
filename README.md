# 1️⃣🐝🏎️ The One Billion Row Challenge in C
I wanted to provide some benchmarks with C code and challenge implementations in other languages.

## Build the binary
gcc -O3 -o 1brc 1brc.c -lpthread

## Run the challenge

## Results
I am limited by the resources of my laptop which is Intel 4-core i7-9750H CPU @ 2.60GHz/ with 6G RAM. So I can only test with 100 million rows.
The first version of my code includes already some basic optimizations and gives an execution of 3.6s. I did further optimizations with multi-threads
on hashing and parsing. The final version gives an execution time of 0.4s.

For comparaison, I took the top 3 java best-performers and another C implementation.
| Result (m:s.ms) | Implementation     | Language | Author |
|-----------------|--------------------|----------|--------|
|       00:00.405 | [link](https://github.com/tieliao/1brc)| C | [Tie Liao](https://github.com/tieliao) |
|       00:00.827 | [link](https://github.com/dannyvankooten/1brc)| C | [Danny van Kooten](https://github.com/dannyvankooten)|
|       00:00.880 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_artsiomkorzun.java)| 21.0.1-graalce  |
|       00:00.945 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_thomaswue.java)| 21.0.1-graalce |
|       00:00.961 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_jerrinot.java)| 21.0.1-graalce   |
|       00:27.131 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_baseline.java)| 21.0.1-graalce  |

For anyone who are interested in running the code with 1 billion rows, please contact me.
