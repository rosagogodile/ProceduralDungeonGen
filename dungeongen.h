/* Rosa Knowles
 * 10/15/2025
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

        // getters
        uint16_t get_width();
        uint16_t get_height();
};

// namespace that stores all tiles and their corresponding IDs
namespace TILES
{
    const uint8_t EMPTY         = '-';
    const uint8_t WALL          = 'X';
    const uint8_t FLOOR         = '0';
    const uint8_t PLAYER_SPAWN  = 'P';
    const uint8_t TREASURE      = 'T';
};

/* Struct that stores a single x, y coordinate pair
 */
struct CoordinatePair
{
    int32_t X;
    int32_t Y;
};

/* Struct that stores the coorindates for a room
 * The 4 coordinates essentially form a bounding box
 */
struct RoomPairs
{
    CoordinatePair top_left;
    CoordinatePair top_right;
    CoordinatePair bottom_left;
    CoordinatePair bottom_right;
    CoordinatePair center;
};

bool check_overlap(RoomPairs a, RoomPairs b);
RoomPairs shift(RoomPairs a, CoordinatePair b);

/* Struct that stores a triangle using three `CoordinatePair` structs
 * Will be used to create the triangulation
 */
struct Triangle
{
    CoordinatePair p1;
    CoordinatePair p2;
    CoordinatePair p3;
};


/* Class that stores the dungeon map
 * Stores a dynamically allocated 2d array, defined in ByteMatrix2D
 */
class DungeonMap
{
    private:
        uint16_t max_room_side_len;
        uint16_t min_room_side_len;
        uint8_t  total_num_rooms;

        std::mt19937            rng;
        std::vector<RoomPairs>  room_coords;

        ByteMatrix2D * matrix_rep = nullptr;

        // private functions that will be called inside of `generate`
        void place_room(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void generate_rooms();
        std::vector<Triangle> Bowyer_Watson();

    public: 
        // constructor
        DungeonMap(uint16_t min_room_len, uint16_t max_room_len, uint8_t num_rooms);
        // destructor
        ~DungeonMap();

        // converts matrix to a string using the tiles
        std::string as_str();
        // generates the dungeon
        void generate(int32_t seed);
};

#endif