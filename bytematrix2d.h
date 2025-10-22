/* Rosa Knowles
 * 10/22/2025
 * Header file for `ByteMatrix2D`, which is a class that stores a 2d matrix of bytes
 */

#ifndef BYTEMATRIX2D_H
#define BYTEMATRIX2D_H

#include <cstdint>
#include <string>
#include <stdexcept>

/* Class that stores a matrix of unsigned 8-bit integers
* Can be an arbitrary size
*/
class ByteMatrix2D
{
    protected:
        // stores the width and height of the matrix
        uint16_t width;
        uint16_t height;

        // dynamically allocated matrix
        uint8_t * matrix = nullptr;

    public:
        // constructor
        ByteMatrix2D(uint16_t w, uint16_t h);
        // default constructor :sob:
        ByteMatrix2D();
        // destructor
        ~ByteMatrix2D();

        // get value at specified coordinates
        uint8_t get(uint16_t x, uint16_t y);
        // set value at specified coordinates
        void set(uint16_t x, uint16_t y, uint8_t val);
        // converts matrix to a string, useful for printing 
        std::string as_str(std::string seperator = "");

        // getters
        uint16_t get_width();
        uint16_t get_height();
};

#endif