#ifndef GRAPH_DUMPER_HPP_
#define GRAPH_DUMPER_HPP_

#include "./graph.hpp"

void dump_graph_to_png_file(const AdjacencyList &adjacency_list,
							const VertexList &vertex_list,
							const char *png_filename);

#endif  // GRAPH_DUMPER_HPP_
