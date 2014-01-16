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

using AdjacencyList =
    std::map<int32_t, std::list<std::tuple<int32_t, int32_t> > >;

using VertexList =
    std::map<int32_t, std::tuple<double, double> >;

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

	for (const auto &vertex: vertex_list) {
		double x, y;
		std::tie(x, y) = std::get<1>(vertex);
		result.minx = std::min(result.minx, x);
		result.miny = std::min(result.miny, y);
		result.maxx = std::max(result.maxx, x);
		result.maxy = std::max(result.maxy, y);
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

void read_adjacency_list(std::istream *is, AdjacencyList *adjacency_list)
{
    int32_t n = read_value<int32_t>(is);
    for (int32_t i = 0; i < n; ++i) {
        int32_t u = read_value<int32_t>(is);
        int32_t m = read_value<int32_t>(is);

        for (int32_t j = 0; j < m; ++j) {
            int32_t v = read_value<int32_t>(is);
            skip<int64_t>(is);
            int32_t t = read_value<int32_t>(is);

            (*adjacency_list)[u].push_back(std::make_tuple(v, t));

            skip<int32_t>(is);
        }
    }
}

void read_vertex_list(std::istream *is, VertexList *vertex_list)
{
    int32_t n = read_value<int32_t>(is);

    for (int32_t i = 0; i < n; ++i) {
        int32_t u = read_value<int32_t>(is);
        skip<int64_t>(is);
        double x = read_value<double>(is) * 0.02;
        double y = read_value<double>(is) * 0.02;

        (*vertex_list)[u] = std::make_tuple(x, y);
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
		for (const auto &vertex: std::get<1>(edge)) {
			auto v = vertex_list.at(std::get<0>(vertex));
			switch (std::get<1>(vertex)) {
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

			cairo_move_to(cr.get(), std::get<0>(u) - bbox.minx, std::get<1>(u) - bbox.miny);
			cairo_line_to(cr.get(), std::get<0>(v) - bbox.minx, std::get<1>(v) - bbox.miny);
			cairo_stroke(cr.get());
		}
	}

	cairo_surface_write_to_png(surface.get(), png_filename);
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
        skip_header(&fin);

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
