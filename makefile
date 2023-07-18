main: main.cpp ./src/*
	g++ -std=c++20 -o main.exe -I ./include main.cpp ./src/*