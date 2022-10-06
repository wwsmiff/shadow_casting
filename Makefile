CC = clang++
CFLAGS = -std=c++17 -pedantic -O3 -Wall -Wextra

main: main.o App.o ShadowCasting.o
	${CC} main.o App.o ShadowCasting.o -o main -lSDL2

main.o: main.cpp
	${CC} ${CFLAGS} -c main.cpp

App.o: App.cpp App.hpp
	${CC} ${CFLAGS} -c App.cpp

ShadowCasting.o: ShadowCasting.hpp ShadowCasting.cpp
	${CC} ${CFLAGS} -c ShadowCasting.cpp

.PHONY: run
run:
	./main

.PHONY: clean
clean:
	rm -f main *.o