#include <cstdint>

#include <istream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <limits>

#include <tuple>
#include <map>
#include <list>

#include <cairo.h>

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

using AdjacencyList =
    std::map<int32_t, std::list<Way> >;

using VertexList =
    std::map<int32_t, Vertex>;

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

struct BoundingBox
{
	BoundingBox(double minx, double miny,
				double maxx, double maxy):
		minx(minx), miny(miny),
		maxx(maxx), maxy(maxy)
	{}

	double minx;
	double miny;
	double maxx;
	double maxy;
};

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

void dump_graph_to_png_file(const AdjacencyList &adjacency_list,
							const VertexList &vertex_list,
							const char *png_filename)
{
	auto bbox = get_bounding_box(vertex_list);

	auto surface = std::shared_ptr<cairo_surface_t>(
		cairo_image_surface_create(CAIRO_FORMAT_RGB24,
								   bbox.maxx - bbox.minx,
								   bbox.maxy - bbox.miny),
		cairo_surface_destroy);

	auto cr = std::shared_ptr<cairo_t>(
		cairo_create(surface.get()),
		cairo_destroy);


	cairo_set_line_width(cr.get(), 1.0);

	for (const auto &edge: adjacency_list) {
		auto u = vertex_list.at(std::get<0>(edge));
		for (const auto &way: std::get<1>(edge)) {
			auto v = vertex_list.at(way.destination);
			switch (way.type) {
			case 0:
				cairo_set_source_rgb(cr.get(), 1.0, 0.0, 0.0);
				break;

			case 1:
				cairo_set_source_rgb(cr.get(), 0.0, 1.0, 0.0);
				break;

			case 2:
				cairo_set_source_rgb(cr.get(), 0.0, 0.0, 1.0);
				break;

			default:
				std::cout << "Wrong type" << std::endl;
			}

			cairo_move_to(cr.get(), u.x - bbox.minx, u.y - bbox.miny);
			cairo_line_to(cr.get(), v.x - bbox.minx, v.y - bbox.miny);
			cairo_stroke(cr.get());
		}
	}

	cairo_surface_write_to_png(surface.get(), png_filename);
}

double distance(const Vertex &p1,
				const Vertex &p2)
{
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	return sqrt(dx * dx + dy * dy);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: ./routing-drawer routing.bin map.png" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        std::ifstream fin(argv[1],
                          std::ifstream::in | std::ifstream::binary);
        auto header = read_header(&fin);

		std::cout << header.transfer_time << std::endl;
		std::cout << header.pedestrian_speed << std::endl;
		std::cout << header.convertion_ratio << std::endl;
		std::cout << header.max_distance << std::endl;

        AdjacencyList adjacency_list;
        read_adjacency_list(&fin, &adjacency_list);

        VertexList vertex_list;
        read_vertex_list(&fin, &vertex_list);

        dump_graph_to_png_file(adjacency_list, vertex_list, argv[2]);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
