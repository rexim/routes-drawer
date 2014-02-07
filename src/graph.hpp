#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <cstdint>
#include <map>
#include <list>

struct Way
{
	Way():
		destination(0),
		path_id(0),
		type(0),
		weight(0)
	{}

	int32_t destination;
	int64_t path_id;
	int32_t type;
	int32_t weight;
};

struct Vertex
{
	Vertex():
		station_id(0),
		x(0.0),
		y(0.0)
	{}

	int64_t station_id;
	double x;
	double y;
};

using AdjacencyList = std::map<int32_t, std::list<Way> >;
using VertexList = std::map<int32_t, Vertex>;

#endif  // GRAPH_HPP_
