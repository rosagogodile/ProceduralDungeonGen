/* Rosa Knowles
 * 10/12/2025
 * Definitions for the methods of `DungeonMap`
 */

#include "dungeongen.h"

// for generating the map:
#include <random>

/* Constructor for the `DungeonMap` class
 * Inherits the `ByteMatrix2D` constructor, and forces the side lengths to be equal
 * Calls the constructor for `ByteMatrix2D`, and then forces each element in the matrix to be the empty tile
 */
DungeonMap::DungeonMap(uint16_t side_len, uint16_t max_room_len) : ByteMatrix2D(side_len, side_len)
{
    // fill matrix with blank tiles 
    for (int i = 0; i < width * height; ++i)
    {
        matrix[i] = TILES::EMPTY;
    }

    // stores the total number of tiles in the dungeon
    // will always be a perfect square!
    num_tiles = side_len * side_len;

    // stores the maximum side length for a room in the dungeon
    max_room_side_len = max_room_len;
}

/* Converts matrix to a string using the tile representations of each of the ids in the matrix
 * Actual implementation is very similar to the `as_str` method for the `ByteMatrix2D` class
 */
std::string DungeonMap::as_tile_str()
{
    using namespace std;

    string rtrnval = "";

    for (uint32_t i = 0; i < num_tiles; ++i)
    {
        const uint8_t VAL = matrix[i];

        // i have no idea if i need to cast to char here, better safe than sorry ig
        rtrnval += char(VAL);

        // adds a newline at the end of a row
        if ((i + 1) % width == 0)
        {
            rtrnval.append("\n");
        }
    }

    // removes the last newline in the string, and returns it
    return rtrnval.erase(rtrnval.size() - 1);
}









#define STARTING_ROOM_SIZE 7



/* Generate the dungeon map
 * Will place tiles based off their corresponding ids in the `TILES` namespace
 * `seed` is a 32-bit integer that defines the random seed for this map generation
 */
void DungeonMap::generate(int32_t seed)
{
    using namespace std;

    // setup random number generator (including setting its seed)
    mt19937 rng(seed);

    // pick position for starting room
    /*
     * TOP-LEFT        = 0
     * TOP-RIGHT       = 1
     * BOTTOM-LEFT     = 2
     * BOTTOM-RIGHT    = 3
     */

    // picks a random value in [0, 3]
    uint8_t starting_pos = rng() % 4;

    // stores the top-left coordinates of the room that the function is currently working on
    uint8_t x = 0, y = 0;

    // find top left coordinate of the room

    // if the starting position is odd, it is on the right
    if (starting_pos % 2 == 1)
    {
        x = width - STARTING_ROOM_SIZE;
    }

    // if starting position is greater than or equal to 2, it is on the bottom
    if (starting_pos >= 2)
    {
        y = height - STARTING_ROOM_SIZE;
    }

    // fill the starting room
    for (uint16_t i = 0; i < STARTING_ROOM_SIZE; ++i)
    {
        for (uint16_t j = 0; j < STARTING_ROOM_SIZE; ++j)
        {
            // places walls around the room
            if (j == 0 || j == STARTING_ROOM_SIZE - 1
                || i == 0 || i == STARTING_ROOM_SIZE - 1)
                set(x + i, y + j, TILES::WALL);
            else
                set(x + i, y + j, TILES::FLOOR);
        }
    }

    // place player spawn and treasure chest in starting room
    // if starting position is less than 2, it is on the top
    // use ternary operator to make sure that when room is at bottom, player is at the bottom of the room
    // and wehn the room is at the top, player is at the top of the room
    set(x + STARTING_ROOM_SIZE / 2, y + ((starting_pos < 2) ? 1 : 3), TILES::PLAYER_SPAWN);
    // place treasure chest in center of room
    set(x + STARTING_ROOM_SIZE / 2, y + STARTING_ROOM_SIZE / 2, TILES::TREASURE);
}