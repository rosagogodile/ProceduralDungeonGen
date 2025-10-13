/* Rosa Knowles
 * 10/13/2025
 * Definitions for the methods of `ByteMatrix2D`
 */

#include "dungeongen.h"

/* Constructor for the `ByteMatrix2D` class
 * dynamically allocates a 2D array based off of the parameters given
 */
ByteMatrix2D::ByteMatrix2D(uint16_t w, uint16_t h)
{
    // store width and height, used for bounds checking
    width = w;
    height = h;

    // allocate the memory for the matrix
    // allocates "width * height" bytes
    // `{0}` ensures each of the bytes are initialized to 0
    matrix = new uint8_t[w * h]{0};
}

/* Default constructor for the `ByteMatrix2D` class
 * Exists just so the compiler won't yell at me 
 * Initializes everything to 0 or nullptr
 */
ByteMatrix2D::ByteMatrix2D()
{
    width = 0;
    height = 0;
    matrix = nullptr;
}

/* Destructor for the `ByteMatrix2D` class
 * ensures `matrix` is freed
 */
ByteMatrix2D::~ByteMatrix2D()
{
    // square brackets used here since `matrix` is an array
    if (matrix != nullptr)
        delete[] matrix;
}

/* Returns the value at a specified coordinate in `matrix`
 * Throws an `std::out_of_range` exception if the bounds are out of range
 */
uint8_t ByteMatrix2D::get(uint16_t x, uint16_t y)
{
    using namespace std; 


    // bounds checking!
    // if the input coordinates are out of the valid range, throw an `std::out_of_range` exception
    // with the text: "Coordinate (`x`, `y`) out of range for ByteMatrix2D of size `width` x `height`"
    if (x >= width || y >= height)
    {
        throw out_of_range("Coordinate (" + to_string(x) + ", " + to_string(y) 
            + ") out of range for ByteMatrix2D of size " 
            + to_string(width) + " x " + to_string(height));
    }

    // moves the pointer to the `x`th row, and the `y`th column (i think)
    return *(matrix + (width * y) + x);
}

/* Sets value at a specified coordinate in `matrix`
 * Throws an `std::out_of_range` exception if the bounds are out of range
 */
void ByteMatrix2D::set(uint16_t x, uint16_t y, uint8_t val)
{
    using namespace std;

    // same bounds checking as in `ByteMatrix2D::get`
    if (x >= width || y >= height)
    {
        throw out_of_range("Coordinate (" + to_string(x) + ", " + to_string(y) 
            + ") out of range for ByteMatrix2D of size " 
            + to_string(width) + " x " + to_string(height));        
    }

    // same pointer math as in `ByteMatrix2D::get`
    *(matrix + (width * y) + x) = val;
}

/* Converts `matrix` to a string
 * has a parameter `seperator`, which will be placed inbetween each element in the matrix
 * `seperator` has a default value of ""
 * Each element in the matrix will be printed as its integer representation, instead of its ASCII representation
 */
std::string ByteMatrix2D::as_str(std::string seperator)
{
    using namespace std;

    string rtrnval = "";

    for (uint32_t i = 0; i < width * height; ++i)
    {
        const uint8_t VAL = matrix[i]; 
        // add leading zeroes
        // all bytes are max 3 digits, so we only need to account for the cases where
        // VAL < 10 and VAL < 100
        if (VAL < 10)
        {
            rtrnval.append("00");
        }
        else if (VAL < 100)
        {
            rtrnval.append("0");
        }

        rtrnval.append(to_string(VAL));
        rtrnval.append(seperator);

        // adds a newline at the end of a row
        if ((i + 1) % width == 0)
        {
            rtrnval.append("\n");
        }
    }

    // removes the last newline in the string, and returns it
    return rtrnval.erase(rtrnval.size() - 1);
}

// Getters
// (self explanatory)
uint16_t ByteMatrix2D::get_width()
{
    return width;
}
uint16_t ByteMatrix2D::get_height()
{
    return height;
}