/* Rosa Knowles
 * 10/13/2025
 * Definitions for the methods of `DungeonMap`
 */

#include "dungeongen.h"

// define this if the program is testing
#define TESTING

#ifdef TESTING
#include <iostream>
#endif

/* Constructor for the `DungeonMap` class
 * Inherits the `ByteMatrix2D` constructor, and forces the side lengths to be equal
 * Calls the constructor for `ByteMatrix2D`, and then forces each element in the matrix to be the empty tile
 */
DungeonMap::DungeonMap(uint16_t side_len, uint16_t min_room_len, uint16_t max_room_len) : ByteMatrix2D(side_len, side_len)
{
    // fill matrix with blank tiles 
    for (int i = 0; i < width * height; ++i)
    {
        matrix[i] = TILES::EMPTY;
    }

    // stores the total number of tiles in the dungeon
    // will always be a perfect square!
    num_tiles = side_len * side_len;
    blank_tiles = num_tiles;

    // stores the maximum side length for a room in the dungeon
    max_room_side_len = max_room_len;
    // stores the minimum side length for a room in the dungeon 
    min_room_side_len = min_room_len;

    room_coords = {};
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

/* Private function
 * places a room in the matrix
 * x -> x-coord of top-left corner of room
 * y -> y-coord of top-left corner of room
 * w -> width of room
 * h -> height of room
 */
void DungeonMap::place_room(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    using namespace std; 

    // place room in the matrix
    // only places the general shape of the room
    /*
    for (uint16_t i = 0; i < w; ++i)
    {
        for (uint16_t j = 0; j < h; ++j)
        {
            // only update tile if the tile is empty
            if (get(x + i, y + j) == TILES::EMPTY)
            {
                // place walls around the room
                if ((j * i) == 0 || j == h - 1 || i == w - 1)
                    set(x + i, y + j, TILES::WALL);
                else
                    set(x + i, y + j, TILES::FLOOR);
            }
        }
    }
    */

    // place room coordinates in vector of room coordinates
    RoomPairs temp;
    // initialize coordinate pairs using struct initializer lists
    temp.top_left = {x, y};
    temp.top_right = {(uint16_t)(x + w), y};
    temp.bottom_left = {x , (uint16_t)(y + h)};
    temp.bottom_right = {(uint16_t)(x + w), (uint16_t)(y + h)};
    temp.middle = {(uint16_t)((x + x + w) / 2), (uint16_t)((y + y + h) / 2)};
    room_coords.push_back(temp);

    // remove number of tiles placed from the total number of blank tiles
    blank_tiles -= w * h;
}



/* Generate the dungeon map
 * Will place tiles based off their corresponding ids in the `TILES` namespace
 * `seed` is a 32-bit integer that defines the random seed for this map generation
 */
void DungeonMap::generate(int32_t seed)
{
    using namespace std;

    // setup random number generator (including setting its seed)
    rng = mt19937(seed);

    // the minimum number of blank tiles needed for the placement loop to continue
    const uint32_t MIN_BLANK_TILES = max_room_side_len * max_room_side_len;

    // stores the top-left coordinates of the room that the function is currently working on
    // always starts in the top-left corner
    uint16_t x_coord = 0, y_coord = 0;
    
    // generate random size for the room
    uint8_t room_width  = rng() % (max_room_side_len - min_room_side_len + 1) + min_room_side_len;
    uint8_t room_height = rng() % (max_room_side_len - min_room_side_len + 1) + min_room_side_len;

    // fill in the first room
    place_room(x_coord, y_coord, room_width, room_height);

    // print room coordinates, used for testing
    #ifdef TESTING
    for (RoomPairs rp : room_coords)
    {
        cout << "(" << to_string(rp.top_left.X) << ", " << to_string(rp.top_left.Y) << "), ";
        cout << "(" << to_string(rp.top_right.X) << ", " << to_string(rp.top_right.Y) << "), ";
        cout << "(" << to_string(rp.bottom_left.X) << ", " << to_string(rp.bottom_left.Y) << "), ";
        cout << "(" << to_string(rp.bottom_right.X) << ", " << to_string(rp.bottom_right.Y) << "), ";
        cout << "(" << to_string(rp.middle.X) << ", " << to_string(rp.middle.Y) << ")" << endl;
    }
    #endif
}