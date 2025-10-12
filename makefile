# Rosa Knowles 
# 10/12/2025
# Makefile for the procedural dungeon generator

OUTPUT_FOLDER := out
TARGET := main

# compiles and then runs the program
run: $(OUTPUT_FOLDER)/$(TARGET).exe
	./$(OUTPUT_FOLDER)/$(TARGET).exe

# compiles the dungeon gen library into an object file
# packs it into a static library using the archiver command 
# removes the object file
DUNGEONGEN_FILES := bytematrix2d.cpp dungeonmap.cpp
DUNGEONGEN_OBJS  := $(DUNGEONGEN_FILES:.cpp=.o)
$(OUTPUT_FOLDER)/libdungeongen.a: dungeongen.h $(DUNGEONGEN_FILES)
	g++ -c $(DUNGEONGEN_FILES)
	ar rcs $(OUTPUT_FOLDER)/libdungeongen.a $(DUNGEONGEN_OBJS)
	rm -f *.o

# compiles the program
$(OUTPUT_FOLDER)/$(TARGET).exe: main.cpp $(OUTPUT_FOLDER)/libdungeongen.a
	g++ -Wall -o $(OUTPUT_FOLDER)/$(TARGET).exe main.cpp -L ./$(OUTPUT_FOLDER) -ldungeongen

# removes all compiled executables and libraries 
clean:
	rm -f $(OUTPUT_FOLDER)/$(TARGET).exe
	rm -f $(OUTPUT_FOLDER)/*.a