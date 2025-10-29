/* Rosa Knowles
 * 10/27/2025
 * Definitions for the methods of `DungeonMap`
 */

#include "dungeongen.h"

// only include `iostream` if testing
// otherwise, `iostream` isn't needed
#ifdef TESTING
    #include <iostream>
#endif


// Takes in two `CoordinatePair` structs
// Returns the distance between the two points as a double
double dist(CoordinatePair x1, CoordinatePair x2)
{
    return sqrt(pow(x1.X - x2.X, 2.0) + pow(x1.Y - x2.Y, 2.0));
}


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


/* Operator overloads
 * Overloads == for `CoordinatePair` and `Triangle`
 * Overloads != for `CoordinatePair`
 */

bool operator==(const CoordinatePair & a, const CoordinatePair & b)
{
    return (a.X == b.X) && (a.Y == b.Y);
}

bool operator!=(const CoordinatePair & a, const CoordinatePair & b)
{
    return !(a == b);
}

bool operator==(const Triangle & a, const Triangle & b)
{
    return (a.p1 == b.p1) && (a.p2 == b.p2) && (a.p3 == b.p3);
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
 * Create Delaunay Triangulation Graph of Rooms
 *      - Essentially, this is a triangular mesh that will have the center of each room as a vertex
 *      - I will use the Bowyer-Watson algorithm for this: https://paulbourke.net/papers/triangulate/
 *      - Convert list of triangles into a graph!!!
 * Create a Minimum Spanning Tree from the triangulation
 *      - I will use Prim's algorithm for this: https://www.w3schools.com/dsa/dsa_algo_mst_prim.php
 * Randomly add connections from the Triangulation Graph into the tree
 * Use A* algorithm to find shortest path between each connected room, and build hallways from this
 * Use the vector of room coordinates to place objects in each of the rooms 

 */



/* Private Function
 * PART 1
 * Place rooms so they don't overlap
 * Generate rooms, and randomly place them until they don't overlap
 */
void DungeonMap::generate_rooms()
{
    using namespace std;

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
    // BUGFIX 10/22/2025:
    // 3 works only if the total number of rooms is greater than or equal to the maximum side length of the room
    // otherwise, we divide by 2 instead
    const uint16_t MAX_SHIFT = (total_num_rooms >= max_room_side_len) ? 
        max_room_side_len * total_num_rooms / 3 :
        max_room_side_len * total_num_rooms / 2;

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
                (int32_t)(rng() % MAX_SHIFT),
                (int32_t)(rng() % MAX_SHIFT)
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

    // add padding to the matrix, for when hallways need to be generated
    matr_sz.X += 2 * PADDING;
    matr_sz.Y += 2 * PADDING;

    CoordinatePair pad_shifter = {PADDING, PADDING};

    for (auto & rp : room_coords)
    {
        rp = shift(rp, pad_shifter);
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
    // triple nested for loop makes me want to kill myself lowkey highkey
    for (auto rp : room_coords)
    {
        x_coord = rp.top_left.X;
        y_coord = rp.top_left.Y;

        for (uint16_t i = 0; i < (rp.bottom_right.Y - y_coord); ++i)
        {
            for (uint16_t j = 0; j < (rp.bottom_right.X - x_coord); ++j)
            {
                // place walls on the outside of the rooms, floors on the inside
                if (i * j == 0 || i == rp.bottom_right.Y - y_coord - 1 || j == rp.bottom_right.X - x_coord - 1)
                    matrix_rep->set(x_coord + j, y_coord + i, TILES::WALL);
                else
                    matrix_rep->set(x_coord + j, y_coord + i, TILES::FLOOR);
            }
        }
    }
}



// blank namespace b/c these should only be used within this file
namespace
{
    // struct that stores an edge (two points)
    // only used within the bowyer-watson algorithm
    struct Edge
    {
        CoordinatePair a;
        CoordinatePair b;

        // overloaded equality operator for `Edge` struct
        friend bool operator==(const Edge & x, const Edge & y)
        {
            return (x.a == y.a) && (x.b == y.b);
        }
    };

    // constant used for floating point comparisons
    const double EPSILON = 1e-4;
};


/* Private Function
 * PART 2
 * Bowyer-Watson algorithm to create Delaunay Triangulation
 * Returns a vector of `Triangle` structs
 */
std::vector<Triangle> DungeonMap::Bowyer_Watson()
{
    // https://paulbourke.net/papers/triangulate/
    using namespace std;

    // initialize and fill vertex list
    // the vertex list will contain the center point of all the rooms
    vector<CoordinatePair> vertex_list;

    for (auto rp : room_coords)
    {
        vertex_list.push_back(rp.center);
    }


    vector<Triangle> triangle_list;

    // DETERMINE SUPER TRIANGLE
    // From a square of a * b (with a on the x-axis and b on the y-axis), we can construct a super triangle
    // with a vertex at (-a/2, 0), (a/2, 2b), (3a/2, 0)
    Triangle super_triangle =
        // use initializer lists to create each of the vertices
        { {(-1 * matrix_rep->get_width()) / 2, 0}, 
        {matrix_rep->get_width() / 2, 2 * matrix_rep->get_height()},
        {3 * matrix_rep->get_width() / 2, 0} };

    // slap super triangle vertices into the vertex list
    vertex_list.push_back(super_triangle.p1);
    vertex_list.push_back(super_triangle.p2);
    vertex_list.push_back(super_triangle.p3);

    triangle_list.push_back(super_triangle);

    for (auto vertex : vertex_list)
    {
        // list that stores valid edges
        vector<Edge> edge_buffer;

        // list that stores the new collection of triangles after the algorithm removed invalid triangles
        vector<Triangle> temp_triangle_list = triangle_list; 

        for (auto tr: triangle_list)
        {
            // find circumcircle for `tr`
            // https://en.wikipedia.org/wiki/Circumcircle#Circumcenter_coordinates

            // let p1 = A, p2 = B, p3 = C (from the formula on wikipedia)

            const double DENOMINATOR = 2 * 
            (
                (tr.p1.X * (tr.p2.Y - tr.p3.Y)) +
                (tr.p2.X * (tr.p3.Y - tr.p1.Y)) +
                (tr.p3.X * (tr.p1.Y - tr.p2.Y))
            );

            // Calculate X and Y coordinate of the circumcenter

            const double CCX = 
                (((pow(tr.p1.X, 2.0) + pow(tr.p1.Y, 2.0)) * (tr.p2.Y - tr.p3.Y)) +
                ((pow(tr.p2.X, 2.0) + pow(tr.p2.Y, 2.0)) * (tr.p3.Y - tr.p1.Y)) +
                ((pow(tr.p3.X, 2.0) + pow(tr.p3.Y, 2.0)) * (tr.p1.Y - tr.p2.Y))) / DENOMINATOR;

            const double CCY = 
                (((pow(tr.p1.X, 2.0) + pow(tr.p1.Y, 2.0)) * (tr.p3.X - tr.p2.X)) +
                ((pow(tr.p2.X, 2.0) + pow(tr.p2.Y, 2.0)) * (tr.p1.X - tr.p3.X)) +
                ((pow(tr.p3.X, 2.0) + pow(tr.p3.Y, 2.0)) * (tr.p2.X - tr.p1.X))) / DENOMINATOR;

            // Calculate radius
            // since all three points sit on the circle, the radius will be the distance between the circumcenter and any one of these points
            // I'm choosing to calculate the distance between the circumcenter and `p1` = A
            const double CCR = sqrt(pow(tr.p1.X - CCX, 2.0) + pow(tr.p1.Y - CCY, 2.0));

            // check if `vertex` lies within this circle
            // if the distance between the circumcenter and vetex is less than the circumradius,
            // it is contained within the circumcircle
            const double CCR_VERTEX_DIST = sqrt(pow(vertex.X - CCX, 2.0) + pow(vertex.Y - CCY, 2.0));

            // check if distance between the circumcenter and vertex is less than the circumradius
            if ((CCR_VERTEX_DIST - CCR) <= EPSILON)
            {
                // add 3 triangle edges to edge buffer
                // remove triangle from triangle list

                // make sure the edges are initialzed such that the point with the 
                // smaller x value is first
                // should help to ensure edges aren't duplicated
                Edge e1, e2, e3;

                // i love spamming if/else blocks!!!!
                if (tr.p1.X <= tr.p2.X)
                {
                    e1.a = tr.p1;
                    e1.b = tr.p2;
                }
                else
                {
                    e1.a = tr.p2;
                    e1.b = tr.p1;
                }

                if (tr.p2.X <= tr.p3.X)
                {
                    e2.a = tr.p2;
                    e2.b = tr.p3;
                }
                else
                {
                    e2.a = tr.p3;
                    e2.b = tr.p2;
                }

                if (tr.p1.X <= tr.p3.X)
                {
                    e3.a = tr.p1;
                    e3.b = tr.p3;
                }
                else
                {
                    e3.a = tr.p3;
                    e3.b = tr.p1;
                }

                edge_buffer.push_back(e1);
                edge_buffer.push_back(e2);
                edge_buffer.push_back(e3);


                // remove triangle from triangle list 
                // use erase-remove idiom -> https://www.geeksforgeeks.org/cpp/erase-remove-idiom-in-cpp/
                auto logical_end = remove_if
                (
                    temp_triangle_list.begin(),
                    temp_triangle_list.end(),
                    [tr](Triangle z) { return z == tr; }
                );
                temp_triangle_list.erase(logical_end, temp_triangle_list.end());

            }

        }

        // update original triangle list from the temporary list
        triangle_list.clear();
        triangle_list = move(temp_triangle_list);

        // delete all doubly specified edges from edge buffer -> leaves only edges for enclosing polygon!!!
        // this section of the code is probably very inefficient but idgaf
        // c++ is like... fast. so my code being shitty doesn't actually matter!! :3

        // lambda that checks if an edge is contained within the list more than once
        auto check_if_edge_doubly_specified = [edge_buffer](Edge z)
        {
            uint8_t count = 0;

            for (auto a : edge_buffer)
            {
                if (a == z)
                    count++;

                if (count > 1)
                    return true;
            }

            return false;
        };

        // erase-remove idiom here!!
        auto logical_end = remove_if
        (
            edge_buffer.begin(),
            edge_buffer.end(),
            check_if_edge_doubly_specified
        );
        edge_buffer.erase(logical_end, edge_buffer.end());


        // print edge buffer if testing
        #ifdef TESTING
            cout << "EDGE BUFFER:" << endl;
            for (auto e : edge_buffer)
            {
                cout << "(" << e.a.X << ", " << e.a.Y << ") <--> ("
                << e.b.X << ", " << e.b.Y << ")" << endl;
            }
        #endif

        // add to triangle list all triangles formed between the point and the edges of the enclosing polygon
        for (auto e : edge_buffer)
        {
            Triangle temp;
            temp.p1 = e.a;
            temp.p2 = e.b;
            temp.p3 = vertex;

            triangle_list.push_back(temp);
        }
    }

    // remove triangles that use supertriangle vertices
    vector<Triangle> finished_triangle_list;

    for (auto tr : triangle_list)
    {
        // only add triangle to the triangle list if it doesn't contain any of the 
        // vertices for the super triangle
        if (tr.p1 != super_triangle.p1 && tr.p1 != super_triangle.p2 && tr.p1 != super_triangle.p3 &&
            tr.p2 != super_triangle.p1 && tr.p2 != super_triangle.p2 && tr.p2 != super_triangle.p3 &&
            tr.p3 != super_triangle.p1 && tr.p3 != super_triangle.p2 && tr.p3 != super_triangle.p3)
        {
            finished_triangle_list.push_back(tr);
        }
    }


    // return final list of triangles
    return finished_triangle_list;
}



/* Private Function
 * PART 3
 * Prim's algorithm to create Minimum Spanning Tree
 * Returns an `sg::SimpleGraph<CoordinatePair>`
 */
sg::SimpleGraph<CoordinatePair> DungeonMap::Prim(const sg::SimpleGraph<CoordinatePair> & full_graph)
{
    // https://www.w3schools.com/dsa/dsa_algo_mst_prim.php
    // https://en.wikipedia.org/wiki/Prim%27s_algorithm

    using namespace std;
    using namespace sg;

    // vertex that we can treat as a null value since it will, in practice, never be used in a practical situation
    const CoordinatePair VERTEX_NULL = {INT32_MIN, INT32_MIN};


    // vector of all coordinate pairs in the graph (vertex list)
    vector<CoordinatePair> vertex_list = full_graph.get_data_list();
    // hashmap of all coordinate pairs in the graph and their connections
    unordered_map<CoordinatePair, vector<CoordinatePair>> connections = full_graph.get_connections();

    // minimum spanning tree
    // data points initialized from the elements in `full_graph`
    SimpleGraph<CoordinatePair> mst(vertex_list);
    
    unordered_set<CoordinatePair> explored_vertices;
    // initialize set of unexplored vertices using the vertex list
    unordered_set<CoordinatePair> unexplored_vertices(vertex_list.begin(), vertex_list.end());

    // get starting vertex
    // this is arbitrary, so we will just pick the first element in the vertex list
    CoordinatePair starting_vertex = vertex_list.at(0);
    unexplored_vertices.erase(starting_vertex);
    explored_vertices.insert(starting_vertex);

    while (unexplored_vertices.size() > 0)
    {
        // find the connection with the minimum weight 
        Edge minimum_connection;
        double minimum_weight = DOUBLE_INF;

        for (const auto & vertex : explored_vertices)
        {
            // iterate through each of the points that connect to `vertex`
            for (const auto & cp : connections.at(vertex))
            {
                // only consider a connection if `cp` has not been explored
                if (unexplored_vertices.find(cp) != unexplored_vertices.end())
                {
                    // calculate the distance between `vertex` and `cp`
                    // this will be our weight
                    double temp_weight = dist(vertex, cp);

                    // if a new minimum weight has been found, save it!
                    if (temp_weight < minimum_weight)
                    {
                        minimum_weight = temp_weight;
                        minimum_connection.a = vertex;
                        minimum_connection.b = cp;
                    }
                }
            }

        }

        // move the unexplored point in the new connection from
        // the set of unexplored vertices to the set of explored vertices
        unexplored_vertices.erase(minimum_connection.b);
        explored_vertices.insert(minimum_connection.b);

        mst.mod_connection(minimum_connection.a, minimum_connection.b, CONNECTED);
    }


    // return minimum spanning tree
    return mst;    
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

    // generate empty rooms w/o hallways
    generate_rooms();

    // get list of triangles, this will be converted into a graph
    vector<Triangle> triangle_list = Bowyer_Watson();
    #ifdef TESTING
        cout << "TRIANGLE LIST: " << endl;
        for (auto tr : triangle_list)
        {
            cout << "(" << tr.p1.X << ", " << tr.p1.Y << "), (" 
                 << tr.p2.X << ", " << tr.p2.Y << "), (" 
                 << tr.p3.X << ", " << tr.p3.Y << ")" << endl;
        }
    #endif

    // CONVERT LIST OF TRIANGLES INTO A GRAPH
    // get set of vertices
    unordered_set<CoordinatePair> set_of_vertices;
    for (auto tr : triangle_list)
    {
        set_of_vertices.insert(tr.p1);
        set_of_vertices.insert(tr.p2);
        set_of_vertices.insert(tr.p3);
    }

    // convert set of vertices to a vector
    // https://stackoverflow.com/questions/42519867/efficiently-moving-contents-of-stdunordered-set-to-stdvector
    vector<CoordinatePair> vertex_list;
    vertex_list.reserve(set_of_vertices.size());

    for (auto it = set_of_vertices.begin(); it != set_of_vertices.end(); )
    {
        vertex_list.push_back(move(set_of_vertices.extract(it++).value())); // ew
    }

    #ifdef TESTING
        cout << "VERTEX LIST: " << endl;
        for (auto v : vertex_list)
        {
            cout << "(" << v.X << ", " << v.Y << ")" << endl;
        }
        cout << "THERE ARE " << to_string(vertex_list.size()) << " VERTICES." << endl; 
    #endif

    // the graph of all vertices, and their connections
    // formed from the list of triangles
    sg::SimpleGraph<CoordinatePair> super_graph(vertex_list);
    // initialize connections
    for (auto tr : triangle_list)
    {
        // connects each vertex of the triangle `tr`
        super_graph.mod_connection(tr.p1, tr.p2, sg::CONNECTED);
        super_graph.mod_connection(tr.p1, tr.p3, sg::CONNECTED);
        super_graph.mod_connection(tr.p2, tr.p3, sg::CONNECTED);
    }

    // print graph connections if testing
    #ifdef TESTING
        // get a hashmap of the connections
        unordered_map<CoordinatePair, vector<CoordinatePair>> connections_a = super_graph.get_connections();

        cout << "CONNECTIONS: " << endl;
        for (const auto [key, value] : connections_a)
        {
            cout << "(" << key.X << ", " << key.Y << "): ";

            for (auto cp : value)
            {
                cout << "(" << cp.X << ", " << cp.Y << ") ";
            }
            cout << endl;
        }
    #endif

    sg::SimpleGraph<CoordinatePair> minimum_spanning_tree = Prim(super_graph);

    // print minimum spanning tree connections if testing
    #ifdef TESTING
        // get a hashmap of the connections
        unordered_map<CoordinatePair, vector<CoordinatePair>> connections_b = minimum_spanning_tree.get_connections();

        cout << "CONNECTIONS: " << endl;
        for (const auto [key, value] : connections_b)
        {
            cout << "(" << key.X << ", " << key.Y << "): ";

            for (auto cp : value)
            {
                cout << "(" << cp.X << ", " << cp.Y << ") ";
            }
            cout << endl;
        }
    #endif
}