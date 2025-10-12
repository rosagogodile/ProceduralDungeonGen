/* Rosa Knowles
 * 10/12/2025
 * Header file for classes, functions, and constants that are used for generating dungeon maps
 */

#ifndef DUNGEONGEN_H
#define DUNGEONGEN_H

// used for fixed width integer types
#include <cstdint>
// strings! 
#include <string>
#include <iostream>

/* Class that stores a matrix of unsigned 8-bit integers
 * Can be an arbitrary size
 */
class ByteMatrix2D
{
    private:
        // stores the width and height of the matrix
        uint16_t width;
        uint16_t height;

        // dynamically allocated matrix
        uint8_t * matrix = nullptr;

    public:
        // constructor
        ByteMatrix2D(uint16_t w, uint16_t h);
        // destructor
        ~ByteMatrix2D();

        // get value at specified coordinates
        uint8_t get(uint16_t x, uint16_t y);
        // set value at specified coordinates
        void set(uint16_t x, uint16_t y, uint8_t val);
        // converts matrix to a string, useful for printing 
        std::string as_str(std::string seperator = "");
};

/* Class that stores the dungeon map
 * Stores a dynamically allocated 2d array, defined in ByteMatrix2D
 */
class DungeonMap 
{
    private:
        // stores the tile id at each coordinate in the dungeon
        ByteMatrix2D dungeon_matrix;
};

#endif