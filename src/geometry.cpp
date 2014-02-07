#include <tuple>
#include <limits>
#include <cmath>

#include "./graph.hpp"
#include "./geometry.hpp"

double distance(const Vertex &p1, const Vertex &p2)
{
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	return sqrt(dx * dx + dy * dy);
}

BoundingBox get_bounding_box(const VertexList &vertex_list)
{
	BoundingBox result(std::numeric_limits<double>::max(),
					   std::numeric_limits<double>::max(),
					   std::numeric_limits<double>::min(),
					   std::numeric_limits<double>::min());

	for (const auto &vertex_element: vertex_list) {
		Vertex vertex;
		std::tie(std::ignore, vertex) = vertex_element;
		result.minx = std::min(result.minx, vertex.x);
		result.miny = std::min(result.miny, vertex.y);
		result.maxx = std::max(result.maxx, vertex.x);
		result.maxy = std::max(result.maxy, vertex.y);
	}

	return result;
}
