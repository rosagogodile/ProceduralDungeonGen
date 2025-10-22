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


            // Destructor
            // ensures `adjacency_matrix` is freed
            ~SimpleGraph()
            {
                if (adjacency_matrix != nullptr)
                    delete adjacency_matrix;
            }

            // GETTERS
            std::vector<T> get_data_list()
            {
                return data_list;
            }
            size_t size()
            {
                // NOTE: `graph_size` is the side length of the matrix
                return graph_size;
            }
            std::unordered_map<T, uint16_t> get_index_map()
            {
                return index_map;
            }
            // returns a copy of the adjacency matrix, to prevent any funny business
            ByteMatrix2D get_adjacency_matrix()
            {
                ByteMatrix2D rtrnval(graph_size, graph_size);

                // since the adjacency matrix is diagonally symmetric,
                // the ordering of `i` and `j` shouldn't matter
                for (uint16_t i = 0; i < graph_size; ++i)
                {
                    for (uint16_t j = 0; j < graph_size; ++j)
                    {
                        rtrnval.set(i, j, adjacency_matrix->get(i, j));
                    }
                }

                return rtrnval;
            }


            // modifies a connection between two data points `a` and `b`
            // `is_connected` should either be equal to `sg::NOT_CONNECTED` or `sg::CONNECTED`
            void mod_connection(T a, T b, uint8_t is_connected)
            {
                uint16_t a_index, b_index;

                a_index = index_map.at(a);
                b_index = index_map.at(b);

                // set both possible orderings to be their connection since
                // the adjacency matrix must be diagonally symmetric
                adjacency_matrix->set(a_index, b_index, is_connected);
                adjacency_matrix->set(b_index, a_index, is_connected);
            }


            // returns whether or not `a` and `b` share a connection
            uint8_t is_connected(T a, T b)
            {
                // since the adjacency matrix is diagonally symmetric,
                // the ordering of `a` and `b` is arbitrary,
                // so I opted for `a` going first and `b` going second
                return adjacency_matrix->get(index_map.at(a), index_map.at(b));
            }

            
            // get a vector of connections for just a single data point
            std::vector<T> get_connections_for(T data)
            {
                std::vector<T> connections;

                uint16_t index = index_map.at(data);
                for (uint16_t i = 0; i < graph_size; ++i)
                {
                    // if a connection is found, put it in the list of connections
                    const uint8_t IS_CONNECTED = adjacency_matrix->get(index, i);
                    if (IS_CONNECTED == CONNECTED)
                        connections.push_back(data_list.at(i));
                }

                return connections;
            }


            // get a map of all of the connections
            std::unordered_map<T, std::vector<T>> get_connections()
            {
                std::unordered_map<T, std::vector<T>> rtrnval;

                for (auto x : data_list)
                {
                    std::vector<T> connections;

                    uint16_t x_index = index_map.at(x);
                    for (uint16_t i = 0; i < graph_size; ++i)
                    {
                        // if a connection is found, put the connection in the list of connections
                        const uint8_t IS_CONNECTED = adjacency_matrix->get(x_index, i);
                        if (IS_CONNECTED == CONNECTED)
                            connections.push_back(data_list.at(i));
                    }

                    // add connections to map of all connections
                    rtrnval.insert({x, connections});
                }


                return rtrnval;
            }



    };
};

#endif