# Rosa Knowles 
# 10/12/2025
# Makefile for the procedural dungeon generator

OUTPUT_FOLDER := out

# compiles and then runs the program
run: $(OUTPUT_FOLDER)/main.exe
	./$(OUTPUT_FOLDER)/main.exe

# compiles the dungeon gen library into an object file
# packs it into a static library using the archiver command 
# removes the object file
DUNGEONGEN_FILES := bytematrix2d.cpp
$(OUTPUT_FOLDER)/libdungeongen.a: dungeongen.h $(DUNGEONGEN_FILES)
	g++ -c $(DUNGEONGEN_FILES) -o dungeongen.o
	ar rcs $(OUTPUT_FOLDER)/libdungeongen.a dungeongen.o
	rm -f dungeongen.o

# compiles the program
$(OUTPUT_FOLDER)/main.exe: main.cpp $(OUTPUT_FOLDER)/libdungeongen.a
	g++ -Wall -o $(OUTPUT_FOLDER)/main.exe main.cpp -L ./$(OUTPUT_FOLDER) -ldungeongen

# removes all compiled executables and libraries 
clean:
	rm -f $(OUTPUT_FOLDER)/main.exe
	rm -f $(OUTPUT_FOLDER)/*.a