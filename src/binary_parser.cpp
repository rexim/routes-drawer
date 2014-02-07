#include "./binary_parser.hpp"

void skip_header(std::istream *is)
{
	skip<int32_t>(is);
	skip<double>(is);
	skip<double>(is);
	skip<int32_t>(is);
}

RoutingHeader read_header(std::istream *is)
{
	RoutingHeader result;
	result.transfer_time = read_value<int32_t>(is);
	result.pedestrian_speed = read_value<double>(is);
	result.convertion_ratio = read_value<double>(is);
	result.max_distance = read_value<int32_t>(is);
	return result;
}

void read_adjacency_list(std::istream *is, AdjacencyList *adjacency_list)
{
	int32_t n = read_value<int32_t>(is);
	for (int32_t i = 0; i < n; ++i) {
		int32_t u = read_value<int32_t>(is);
		int32_t m = read_value<int32_t>(is);

		// Ensuring that even empty rows of adjacency list will be
		// created, so adjacency_list->size() is actual amount of
		// rows.
		if (adjacency_list->find(u) == adjacency_list->end()) {
			adjacency_list->insert(std::make_pair(u, std::list<Way>()));
		}

		for (int32_t j = 0; j < m; ++j) {
			Way way;

			way.destination = read_value<int32_t>(is);
			way.path_id = read_value<int64_t>(is);
			way.type = read_value<int32_t>(is);
			way.weight = read_value<int32_t>(is);

			(*adjacency_list)[u].push_back(way);
		}
	}
}

void read_vertex_list(std::istream *is, VertexList *vertex_list)
{
	int32_t n = read_value<int32_t>(is);

	for (int32_t i = 0; i < n; ++i) {
		int32_t u = read_value<int32_t>(is);

		Vertex vertex;
		vertex.station_id = read_value<int64_t>(is);
		vertex.x = read_value<double>(is) * 0.01;
		vertex.y = read_value<double>(is) * 0.01;

		(*vertex_list)[u] = vertex;
	}
}
