all:
	clang++ main.cc scheduler.cc coroutine.cc -std=c++11 -o main -g
