/* Rosa Knowles
 * 10/13/2025
 * Definitions for the methods of `DungeonMap`
 */

#include "dungeongen.h"

// define this if the program is testing
#define TESTING

// only include `iostream` if testing
// otherwise, `iostream` isn't needed
#ifdef TESTING
    #include <iostream>
#endif

/* checks if two `RoomPairs` (a, b) are overlapping
 */
bool check_overlap(RoomPairs a, RoomPairs b)
{
    // lambda that checks if a point is bounded
    auto point_bounded = [](CoordinatePair point, RoomPairs box)
    {
        return
            point.X <= box.top_right.X   &&
            point.X >= box.top_left.X    &&
            point.Y <= box.bottom_left.Y &&
            point.Y >= box.top_left.Y;
    };

    // if any of these evaluate to true, the boxes overlap
    bool rtrnval =  point_bounded(a.bottom_left, b) ||
                    point_bounded(a.bottom_right, b) ||
                    point_bounded(a.center, b) ||
                    point_bounded(a.top_left, b) ||
                    point_bounded(a.top_right, b) ||
                    point_bounded(b.bottom_left, a) ||
                    point_bounded(b.bottom_right, a) ||
                    point_bounded(b.center, a) ||
                    point_bounded(b.top_left, a) ||
                    point_bounded(b.top_right, a);

    // return whether or not the boxes overlap
    return rtrnval;
}

/* Shifts all points in `a` by `b` and returns it as a new `RoomPairs`
 */
RoomPairs shift(RoomPairs a, CoordinatePair b)
{
    RoomPairs rtrnval = a;

    rtrnval.bottom_left.X += b.X;
    rtrnval.bottom_left.Y += b.Y;
    rtrnval.bottom_right.X += b.X;
    rtrnval.bottom_right.Y += b.Y;
    rtrnval.center.X += b.X;
    rtrnval.center.Y += b.Y;
    rtrnval.top_left.X += b.X;
    rtrnval.top_left.Y += b.Y;
    rtrnval.top_right.X += b.X;
    rtrnval.top_right.Y += b.Y;

    return rtrnval;
}


/* Constructor for the `DungeonMap` class
 * Inherits the `ByteMatrix2D` constructor, and forces the side lengths to be equal
 * Calls the constructor for `ByteMatrix2D`, and then forces each element in the matrix to be the empty tile
 */
DungeonMap::DungeonMap(uint16_t min_room_len, uint16_t max_room_len, uint8_t num_rooms)
{
    // stores the maximum side length for a room in the dungeon
    max_room_side_len = max_room_len;
    // stores the minimum side length for a room in the dungeon 
    min_room_side_len = min_room_len;
    // stores the total number of rooms 
    total_num_rooms = num_rooms;

    room_coords = {};
}

/* Destructor for the `DungeonMap` class
 * Ensures that `matrix_rep` is freed
 */
DungeonMap::~DungeonMap()
{
    if (matrix_rep != nullptr)
        delete matrix_rep;
}



/* Converts matrix to a string using the tile representations of each of the ids in the matrix
 * Actual implementation is very similar to the `as_str` method for the `ByteMatrix2D` class
 */
std::string DungeonMap::as_str()
{
    using namespace std;

    string rtrnval = "";

    for (uint16_t i = 0; i < matrix_rep->get_height(); ++i)
    {
        for (uint16_t j = 0; j < matrix_rep->get_width(); ++j)
        {
            const uint8_t VAL = matrix_rep->get(j, i);

            // i have no idea if i need to cast to char here, better safe than sorry ig
            rtrnval += char(VAL);
        }
        // adds a newline at the end of a row
        rtrnval.append("\n");
    }

    // removes the last newline in the string, and returns it
    return rtrnval.erase(rtrnval.size() - 1);
}

/* Private function
 * places a room in the vector of rooms
 * x -> x-coord of top-left corner of room
 * y -> y-coord of top-left corner of room
 * w -> width of room
 * h -> height of room
 */
void DungeonMap::place_room(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    using namespace std; 

    // place room coordinates in vector of room coordinates
    RoomPairs temp;
    // initialize coordinate pairs using struct initializer lists
    temp.top_left = {x, y};
    temp.top_right = {(uint16_t)(x + w), y};
    temp.bottom_left = {x , (uint16_t)(y + h)};
    temp.bottom_right = {(uint16_t)(x + w), (uint16_t)(y + h)};
    temp.center = {(uint16_t)((x + x + w) / 2), (uint16_t)((y + y + h) / 2)};
    room_coords.push_back(temp);
}


/* Pseudocode: 
 * https://vazgriz.com/119/procedurally-generated-dungeons/
 
 * Place rooms such that they don't overlap
 * Create Delaunay Triangulation Graph of Rooms(???)
 *      - Essentially, this is a triangular mesh that will have the center of each room as a vertex
 *      - I will use the Bowyer-Watson algorithm for this
 * Create a Minimum Spanning Tree from the triangulation
 *      - I will use Prim's algorithm for this
 * Randomly add connections from the Triangulation Graph into the tree
 * Use A* algorithm to find shortest path between each connected room, and build hallways from this

 */


/* Generate the dungeon map
 * Will place tiles based off their corresponding ids in the `TILES` namespace
 * `seed` is a 32-bit integer that defines the random seed for this map generation
 */
void DungeonMap::generate(int32_t seed)
{
    using namespace std;

    // setup random number generator (including setting its seed)
    rng = mt19937(seed);

    // stores the top-left coordinates of the room that the function is currently working on
    // always starts in the top-left corner
    uint16_t x_coord = 0, y_coord = 0;

    for (uint8_t i = 0; i < total_num_rooms; ++i)
    {
        // generate random size for the room
        uint8_t room_width  = rng() % (max_room_side_len - min_room_side_len + 1) + min_room_side_len;
        uint8_t room_height = rng() % (max_room_side_len - min_room_side_len + 1) + min_room_side_len;

        // add the room to the room vector
        place_room(x_coord, y_coord, room_width, room_height);
    }

    // create a temporary vector to store all shifted room coords
    vector<RoomPairs> temp_vec = {};

    // maximum shift size
    // 3 seems to be the magic number here, any higher and the dungeon isn't garunteed to generate.
    // any lower and the dungeon feels too spread out
    const uint16_t MAX_SHIFT = (max_room_side_len * total_num_rooms) / 3;

    // lambda that checks if the box overlaps with any box in a vector of boxes
    auto box_overlaps = [](vector<RoomPairs> a, RoomPairs b)
    {
        if (a.size() == 0)
            return false;

        bool rtrnval = false;

        for (auto rp : a)
        {
            rtrnval = rtrnval || check_overlap(rp, b);
        }

        return rtrnval;
    };

    #ifdef TESTING
        int rand_count = 0;
    #endif

    // shift all rooms so they don't overlap with each other
    for (auto rp : room_coords)
    {
        RoomPairs temp_rp;

        do
        {
            // picks a coordinate pair (x, y) such that
            // x and y are in [-MAX_SHIFT, MAX_SHIFT]
            CoordinatePair shifter = 
            {
                // casting to `int32_t` here so g++ doesn't yell at me :P
                (uint16_t)(rng() % MAX_SHIFT),
                (uint16_t)(rng() % MAX_SHIFT)
            };

            temp_rp = shift(rp, shifter);

            #ifdef TESTING
                rand_count++;
            #endif

        }
        while (box_overlaps(temp_vec, temp_rp));

        temp_vec.push_back(temp_rp);


    }

    #ifdef TESTING
        cout << "Number of repetitions: " << to_string(rand_count) << endl;
    #endif

    // since `temp_vec` now contains all the rooms such that their positions don't overlap, replace `room_coords` with it
    room_coords = temp_vec;

    // find the size of the matrix
    CoordinatePair matr_sz = {0, 0};

    // NOTE: bottom right coordinate will have the greatest position
    for (auto rp : room_coords)
    {
        if (rp.bottom_right.X > matr_sz.X)
            matr_sz.X = rp.bottom_right.X;
        if (rp.bottom_right.Y > matr_sz.Y)
            matr_sz.Y = rp.bottom_right.Y;
    }

    #ifdef TESTING
        cout << "Matrix size: " << to_string(matr_sz.X) << " * " << to_string(matr_sz.Y) << endl << endl;
    #endif

    // print room coordinates, used for testing
    #ifdef TESTING
        for (auto rp : room_coords)
        {
            cout << "(" << to_string(rp.top_left.X) << ", " << to_string(rp.top_left.Y) << "), ";
            cout << "(" << to_string(rp.top_right.X) << ", " << to_string(rp.top_right.Y) << "), ";
            cout << "(" << to_string(rp.bottom_left.X) << ", " << to_string(rp.bottom_left.Y) << "), ";
            cout << "(" << to_string(rp.bottom_right.X) << ", " << to_string(rp.bottom_right.Y) << "), ";
            cout << "(" << to_string(rp.center.X) << ", " << to_string(rp.center.Y) << ")" << endl;
        }
    #endif

    // Create and populate matrix!

    matrix_rep = new ByteMatrix2D(matr_sz.X, matr_sz.Y);

    // fill matrix with empty tiles
    for (uint16_t i = 0; i < matrix_rep->get_width(); ++i)
    {
        for (uint16_t j = 0; j < matrix_rep->get_height(); ++j)
        {
            matrix_rep->set(i, j, TILES::EMPTY);
        }
    }

    // place rooms in matrix
    for (auto rp : room_coords)
    {
        x_coord = rp.top_left.X;
        y_coord = rp.top_left.Y;

        for (uint16_t i = 0; i < (rp.bottom_right.Y - y_coord); ++i)
        {
            for (uint16_t j = 0; j < (rp.bottom_right.X - x_coord); ++j)
            {
                matrix_rep->set(x_coord + j, y_coord + i, TILES::FLOOR);
            }
        }
    }
}