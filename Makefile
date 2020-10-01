CC = g++
CFLAGS = -Wall -std=c++11

all: main

main: main.cpp
	$(CC) -g $(CFLAGS) -o main main.cpp

clean: 
	-rm main
