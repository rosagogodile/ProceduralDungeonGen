# Rosa Knowles 
# 10/12/2025
# Makefile for the procedural dungeon generator

# compiles and then runs the program
run: main.exe
	./main.exe

# compiles the dungeon gen library into an object file
# packs it into a static library using the archiver command 
# removes the object file
DUNGEONGEN_FILES := bytematrix2d.cpp
libdungeongen.a: dungeongen.h $(DUNGEONGEN_FILES)
	g++ -c $(DUNGEONGEN_FILES) -o dungeongen.o
	ar rcs libdungeongen.a dungeongen.o
	rm -f dungeongen.o

# compiles the program
main.exe: main.cpp libdungeongen.a
	g++ -Wall -o main.exe main.cpp -L . -ldungeongen

# removes all compiled executables and libraries 
clean:
	rm -f main.exe
	rm -f *.a