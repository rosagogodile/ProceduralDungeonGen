# Rosa Knowles 
# 10/13/2025
# Makefile for the procedural dungeon generator

OUTPUT_FOLDER := out
TARGET := main

# compiles and then runs the program
run: $(OUTPUT_FOLDER)/$(TARGET).exe
	./$(OUTPUT_FOLDER)/$(TARGET).exe

# compile and then run the program
# use a redirect to store the output in a txt file
txt: $(OUTPUT_FOLDER)/$(TARGET).exe
	./$(OUTPUT_FOLDER)/$(TARGET).exe > $(OUTPUT_FOLDER)/$(TARGET).txt

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
# also removes all compiled object files, in the event that compilation fails for something else
clean:
	rm -f $(OUTPUT_FOLDER)/$(TARGET).exe
	rm -f $(OUTPUT_FOLDER)/*.a
	rm -f *.o
	rm -f $(OUTPUT_FOLDER)/$(TARGET).txt