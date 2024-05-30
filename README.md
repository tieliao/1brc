# 1️⃣🐝🏎️ The One Billion Row Challenge in C
As promised, I worked out an implementation in C for 1brc. The goal is
to provide some benchmarks with C code and challenge other implementations.
I am specially interested in what performance can be achieved by implementations
in different languages for this particular application.

This implementation shows, by using some simple and comprehensive optimization
techniques (no SIMD and no dependencies), the performance can be considerably increased. 

To know more about 1BRC, here are some pointers:
- What is 1BRC?: https://www.morling.dev/blog/one-billion-row-challenge/
- Git repository: https://github.com/gunnarmorling/1brc

## Cloning this repository
```
git clone https://github.com/tieliao/1brc
```

## Generating raw data
You may need to install jdk 21.
```
git clone https://github.com/gunnarmorling/1brc
cd 1brc
./mvnw clean verify
./create_measurements.sh 100000000
```
This will create a file, namely measurements.txt, with 100 million rows. This
file contains 413 different city names.

To create a file (measurements3.txt) with 10k different city names:
```
./create_measurements3.sh 100000000
```

To test the baseline implementation:
```
time ./calculate_average_baseline.sh > output.txt
```

## Running the challenge

```
make
```
Or run it manually:
```
gcc -O3 -DNDEBUG -o summarize 1brc.c -lpthread
time ./summarize 1brc/measurements.txt $(nproc)
```

## Results
I was limited by the resources of my laptop which is an Intel 4-core i7-9750H CPU @ 2.60GHz/6GB RAM. So I can only test with 100 million rows.
The first version of my code includes already some basic optimizations and gives
an execution time of 3.6s. I did further optimizations with multi-threads,
on hashing and parsing. The final version gives an execution time of 0.4s.

For comparaison, I ran the top 3 java best-performers and another C implementation as well.
| Result (m:s.ms) | Implementation     | Language | Author |
|-----------------|--------------------|----------|--------|
|       00:00.390 | [link](https://github.com/tieliao/1brc)| C | [Tie Liao](https://github.com/tieliao) |
|       00:00.813 | [link](https://github.com/dannyvankooten/1brc)| C | [Danny van Kooten](https://github.com/dannyvankooten)|
|       00:00.880 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_artsiomkorzun.java)| 21.0.1-graalce | [Artsiom Korzun](https://github.com/artsiomkorzun)|
|       00:00.945 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_thomaswue.java)| 21.0.1-graalce | [Thomas Wuerthinger](https://github.com/thomaswue), [Quan Anh Mai](https://github.com/merykitty), [Alfonso² Peterssen](https://github.com/mukel)|
|       00:00.961 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_jerrinot.java)| 21.0.1-graalce  | [Jaromir Hamala](https://github.com/jerrinot)|
|       00:27.131 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_baseline.java)| 21.0.1-graalce  | Baseline [Gunnar Morling](https://github.com/gunnarmorling)|

The above results are using 413 different city names. I also tested with 10k different city names and the results are:
| Result (m:s.ms) | Implementation     | Language | Author |
|-----------------|--------------------|----------|--------|
|       00:00.677 | [link](https://github.com/tieliao/1brc)| C | [Tie Liao](https://github.com/tieliao) |
|       00:01.406 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_artsiomkorzun.java)| 21.0.1-graalce | [Artsiom Korzun](https://github.com/artsiomkorzun)|
|       00:01.440 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_jerrinot.java)| 21.0.1-graalce  | [Jaromir Hamala](https://github.com/jerrinot)|
|       00:01.609 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_thomaswue.java)| 21.0.1-graalce | [Thomas Wuerthinger](https://github.com/thomaswue), [Quan Anh Mai](https://github.com/merykitty), [Alfonso² Peterssen](https://github.com/mukel)|
|       00:02.345 | [link](https://github.com/dannyvankooten/1brc)| C | [Danny van Kooten](https://github.com/dannyvankooten)|
|       00:33.134 | [link](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CalculateAverage_baseline.java)| 21.0.1-graalce  | Baseline [Gunnar Morling](https://github.com/gunnarmorling)|

The accuracy of output was successfully checked against that of baseline in both cases.

For anyone who is interested in running the code with 1 billion rows, please let me know.
