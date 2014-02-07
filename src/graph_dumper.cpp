#include <iostream>
#include <memory>

#include <cairo.h>

#include "./geometry.hpp"
#include "./graph_dumper.hpp"

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
		auto u = vertex_list.find(std::get<0>(edge));

		if (u == vertex_list.end()) {
			std::cout << "[WARNING] " << std::endl;
			continue;
		}

		for (const auto &way: std::get<1>(edge)) {
			auto v = vertex_list.find(way.destination);

			if (v == vertex_list.end()) {
				std::cout << "[WARNING] " << std::endl;
				continue;
			}

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
				std::cout << "[WARNING] Wrong arc type" << std::endl;
			}

			cairo_move_to(cr.get(), u->second.x - bbox.minx, u->second.y - bbox.miny);
			cairo_line_to(cr.get(), v->second.x - bbox.minx, v->second.y - bbox.miny);
			cairo_stroke(cr.get());
		}
	}

	cairo_surface_write_to_png(surface.get(), png_filename);
}
