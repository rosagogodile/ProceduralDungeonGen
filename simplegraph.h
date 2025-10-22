/* Rosa Knowles
 * 10/22/2025
 * Header file for a simple implementation of a graph
 * Uses an adjacency matrix
 * https://www.w3schools.com/dsa/dsa_data_graphs_implementation.php
 */

#ifndef SIMPLE_GRAPH_H
#define SIMPLE_GRAPH_H

#include <vector>
#include <cstdint>
#include <unordered_map>

#include "bytematrix2d.h"



namespace sg
{
    const uint8_t NOT_CONNECTED = 0;
    const uint8_t CONNECTED     = 1;


    template <typename T>
    class SimpleGraph
    {
        private:
            // stores the pairings in the graph
            ByteMatrix2D * adjacency_matrix = nullptr;
            // stores the size of the graph
            size_t graph_size;
            // stores the actual data in the graph
            std::vector<T> data_list;
            // stores the pairings between the data and their index in `data_list`
            std::unordered_map<T, uint16_t> index_map;

        public:
            // constructor
            SimpleGraph(std::vector<T> data_list_arg)
            {
                data_list = data_list_arg;

                graph_size = data_list.size();

                // initializes a new square matrix 
                // will store which data points are adjacent with each other
                adjacency_matrix = new ByteMatrix2D(graph_size, graph_size);

                // NOTE: the constructor for `ByteMatrix2D` initializes all values to 0 for me, 
                // so I don't need to worry about doing that here

                // initialize `index_map`
                // makes the assumption that each element in `data_list` is unique
                // there may be some weird behavior if there are non-unique elements,
                // but for my purposes, I shouldn't have non-unique elements
                for (uint16_t i = 0; i < graph_size; ++i)
                {
                    // pairs a data point with its index for fast searching
                    index_map.insert({data_list.at(i), i});
                }

            }

    };
};

#endif