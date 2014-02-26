#ifndef BINARY_PARSER_HPP_
#define BINARY_PARSER_HPP_

#include <cstdint>
#include <istream>

#include "./graph.hpp"

struct RoutingHeader
{
    RoutingHeader():
        transfer_time(0),
        pedestrian_speed(0.0),
        convertion_ratio(0.0),
        max_distance(0)
    {}

    int32_t transfer_time;
    double pedestrian_speed;
    double convertion_ratio;
    int32_t max_distance;
};

template <typename Type>
Type read_value(std::istream *is)
{
    int n = sizeof(Type);
    char buffer[n];

    for (int i = n - 1; i >= 0; --i) {
        buffer[i] = is->get();
    }

    return *reinterpret_cast<Type*>(buffer);
}

template <typename Type>
void skip(std::istream *is)
{
    read_value<Type>(is);
}

void skip_header(std::istream *is);
RoutingHeader read_header(std::istream *is);
void read_adjacency_list(std::istream *is, AdjacencyList *adjacency_list);
void read_vertex_list(std::istream *is, VertexList *vertex_list);

#endif  // BINARY_PARSER_HPP_
