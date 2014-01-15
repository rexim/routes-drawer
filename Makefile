routing-drawer: main.cpp Makefile
	g++ -std=c++11 `pkg-config cairo --cflags` -Wall -Werror -Wno-strict-aliasing -O2 -ggdb -o routing-drawer main.cpp `pkg-config cairo --libs`
