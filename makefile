all: 
   g++ -std=c++17 -Wall -g -pthread mt-collatz.cpp -o mt-collatz
clean:
   rm -f mt-collatz
