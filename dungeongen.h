/* Rosa Knowles
 * 10/25/2025
 * Header file for classes, functions, and constants that are used for generating dungeon maps
 */

#ifndef DUNGEONGEN_H
#define DUNGEONGEN_H

// INCLUDES

// used for fixed width integer types
#include <cstdint>
#include <climits> // needed for linux i think?
#include <limits>
// strings! 
#include <string>
#include <random>
#include <vector>
// mainly used for calculating the circumcircle of a triangle
#include <cmath>
#include <algorithm>
#include <utility>
#include <functional>
#include <unordered_set>
#include <unordered_map>
// #include <tuple>

#include "bytematrix2d.h"
#include "simplegraph.h"

// ------

// DEFINES

// the amount of padding at the edge of the matrix after the rooms have been generated
// needed in order to ensure that there is enough space for each of the hallways
#define PADDING 5

// infinity for doubles
// from `<limits>`
#define DOUBLE_INF std::numeric_limits<double>::infinity()

// define this if the program is being tested
// if this is defined, extra info will be printed out from the functions defined in `dungeonmap.cpp`
#define TESTING

// ------



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

    friend bool operator==(const CoordinatePair & a, const CoordinatePair & b);
    friend bool operator!=(const CoordinatePair & a, const CoordinatePair & b);
};

/* Makes `CoordinatePair` hashable
 * https://www.delftstack.com/howto/cpp/hash-in-cpp/
 * This feels really hacky??? But it works so I'm okay with it??????
 */
namespace std
{
    template <>
    struct hash<CoordinatePair>
    {
        size_t operator()(const CoordinatePair & cp) const
        {
            // https://stackoverflow.com/questions/23933227/what-is-a-fast-hash-function-for-pairs-of-ints
            // note that `INT_MAX + 1` == `INT_MIN`
            return (hash<int>()(cp.X) * (INT_MIN)) + hash<int>()(cp.Y);
        }
    };

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

    friend bool operator==(const Triangle & a, const Triangle & b);
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
        sg::SimpleGraph<CoordinatePair> Prim(const sg::SimpleGraph<CoordinatePair> & full_graph);

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