/* Rosa Knowles
 * 10/13/2025
 * Header file for classes, functions, and constants that are used for generating dungeon maps
 */

#ifndef DUNGEONGEN_H
#define DUNGEONGEN_H

// used for fixed width integer types
#include <cstdint>
// strings! 
#include <string>
#include <stdexcept>
#include <random>
#include <vector>

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
};

// namespace that stores all tiles and their corresponding IDs
namespace TILES
{
    const uint8_t EMPTY         = ' ';
    const uint8_t WALL          = '~';
    const uint8_t FLOOR         = '0';
    const uint8_t PLAYER_SPAWN  = 'P';
    const uint8_t TREASURE      = 'T';
    const uint8_t DOOR          = 'D';
};

/* Struct that stores a single x, y coordinate pair
 */
struct coordinate_pair
{
    uint16_t X;
    uint16_t Y;
};

// struct room_pairs
// {
//     coordinate_pair top_left;
//     coordinate_pair top_right;
//     coordinate_pair bottom_left;
//     coordinate_pair bottom_right;
// };


/* Class that stores the dungeon map
 * Stores a dynamically allocated 2d array, defined in ByteMatrix2D
 */
class DungeonMap: public ByteMatrix2D
{
    private:
        uint16_t max_room_side_len;
        uint16_t min_room_side_len;
        uint32_t num_tiles;

        std::mt19937 rng;

        void place_room(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool downwards);

        std::vector<coordinate_pair> room_coords;

    public: 
        // constructor
        DungeonMap(uint16_t side_len, uint16_t min_room_len, uint16_t max_room_len);

        // converts matrix to a string using the tiles
        std::string as_tile_str();
        // generates the dungeon
        void generate(int32_t seed);
};

#endif