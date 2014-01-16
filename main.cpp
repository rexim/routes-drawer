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

void read_vertex_list(std::istream *is, VertexList *vertex_list,
                      double &minx, double &miny,
                      double &maxx, double &maxy)
{
    minx = std::numeric_limits<double>::max();
    miny = std::numeric_limits<double>::max();
    maxx = std::numeric_limits<double>::min();
    maxy = std::numeric_limits<double>::min();

    int32_t n = read_value<int32_t>(is);

    for (int32_t i = 0; i < n; ++i) {
        int32_t u = read_value<int32_t>(is);
        skip<int64_t>(is);
        double x = read_value<double>(is) * 0.01;
        double y = read_value<double>(is) * 0.01;

        (*vertex_list)[u] = std::make_tuple(x, y);
        minx = std::min(minx, x);
        miny = std::min(miny, y);
        maxx = std::max(maxx, x);
        maxy = std::max(maxy, y);
    }

	minx -= 10; maxx += 10;
	miny -= 10; maxy += 10;
}

void dump_graph_to_png_file(const AdjacencyList &adjacency_list,
							const VertexList &vertex_list,
							double minx, double miny, double maxx, double maxy,
							const char *png_filename)
{
	auto surface = std::shared_ptr<cairo_surface_t>(
		cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
								   maxx - minx,
								   maxy - miny),
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

			cairo_move_to(cr.get(), std::get<0>(u) - minx, std::get<1>(u) - miny);
			cairo_line_to(cr.get(), std::get<0>(v) - minx, std::get<1>(v) - miny);
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
        double minx, miny, maxx, maxy;
        read_vertex_list(&fin, &vertex_list, minx, miny, maxx, maxy);

        dump_graph_to_png_file(adjacency_list, vertex_list,
							   minx, miny, maxx, maxy,
							   argv[2]);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
