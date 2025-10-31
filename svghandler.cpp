/* Rosa Knowles
 * 10/31/2025
 * Definition for a function that generates an svg graphic from a graph
 * https://www.w3schools.com/graphics/svg_intro.asp
 */

#include "dungeongen.h"

#include <iostream>
#include <fstream>


/* Function that generates an svg graphic of a `SimpleGraph` object containing `CoordinatePair` structs
 * Takes `SimpleGraph<CoordinatePair> graph` as the graph that will be converted into an svg
 * `filepath` is the path to the svg file. must end with ".svg"
 * Returns nothing
 */
void graph_to_svg(const sg::SimpleGraph<CoordinatePair> & graph, std::string filepath)
{
    using namespace std;

    // get list of vertices
    vector<CoordinatePair> vertex_list = graph.get_data_list();
    // get map of connections 
    unordered_map<CoordinatePair, vector<CoordinatePair>> connections = graph.get_connections();

    // find the width and height
    int32_t width, height;
    width = vertex_list.at(0).X;
    height = vertex_list.at(0).Y;

    for (const auto & vertex : vertex_list)
    {
        if (vertex.X > width)  width  = vertex.X;
        if (vertex.Y > height) height = vertex.Y;
    }

    width += SVG_RESOLUTION;
    height += SVG_RESOLUTION;

    // open up svg file
    fstream svg_file(filepath, ios::out | ios::trunc);
    // check if file *actually* opened
    if (!svg_file)
    {
        cerr << "File `" << filepath << "` failed to open." << endl;
        exit(1);
    }

    // open xml block
    svg_file << "<svg width=\"" << SVG_RESOLUTION * width << "\" height=\"" << SVG_RESOLUTION * height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    // add lines to svg file
    for (const auto & vertex : vertex_list)
    {
        // iterate through each connection the vertex has
        for (const auto & c : connections.at(vertex))
        {
            svg_file << "\t<line x1=\"" << SVG_RESOLUTION * vertex.X << "\" y1=\"" << SVG_RESOLUTION * vertex.Y
                     << "\" x2=\"" << SVG_RESOLUTION * c.X << "\" y2=\"" << SVG_RESOLUTION * c.Y << "\" style=\"stroke:"
                     << SVG_CONNECTION_COLOR << ";stroke-width:" << SVG_RESOLUTION / 2 << "\" />\n";

            // remove the connection from future iterations to prevent unnecessary repetition
            auto pos = find(connections.at(c).begin(), connections.at(c).end(), vertex);
            if (pos != connections.at(c).end())
                connections.at(c).erase(pos);
        }
    }

    // add points to svg file
    for (const auto & vertex : vertex_list)
    {
        svg_file << "\t<circle r=\"" << SVG_RESOLUTION << "\" cx=\"" << SVG_RESOLUTION * vertex.X 
                 << "\" cy=\"" << SVG_RESOLUTION * vertex.Y << "\" fill=\"" << SVG_POINT_COLOR << "\" />\n";
    }

    // close xml block
    svg_file << "</svg>";

    // close the svg file
    svg_file.close();
}