#include <iostream>
#include <memory>
#include <exception>
#include <stdexcept>

#include <cairo.h>

#include "./geometry.hpp"
#include "./graph_dumper.hpp"

void check_cairo_surface_status(cairo_surface_t *surface)
{
    auto status = cairo_surface_status(surface);
    if (status != CAIRO_STATUS_SUCCESS) {
        throw std::runtime_error(cairo_status_to_string(status));
    }
}

void check_cairo_status(cairo_t *cr)
{
    auto status = cairo_status(cr);
    if (status != CAIRO_STATUS_SUCCESS) {
        throw std::runtime_error(cairo_status_to_string(status));
    }
}

void dump_graph_to_png_file(const AdjacencyList &adjacency_list,
                            const VertexList &vertex_list,
                            const char *png_filename)
{
    auto bbox = get_aabb(vertex_list);

    double width = bbox.maxx - bbox.minx;
    double height = bbox.maxy - bbox.miny;

    auto surface = std::shared_ptr<cairo_surface_t>(
        cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height),
        cairo_surface_destroy);
    check_cairo_surface_status(surface.get());

    auto cr = std::shared_ptr<cairo_t>(
        cairo_create(surface.get()),
        cairo_destroy);
    check_cairo_status(cr.get());

    cairo_set_line_width(cr.get(), 1.0);

    for (const auto &edge: adjacency_list) {
        auto u = vertex_list.find(std::get<0>(edge));

        if (u == vertex_list.end()) {
            std::cout << "[WARNING] reference to non-existing vertex "
                      << std::get<0>(edge) << std::endl;
            continue;
        }

        for (const auto &way: std::get<1>(edge)) {
            auto v = vertex_list.find(way.destination);

            if (v == vertex_list.end()) {
                std::cout << "[WARNING] reference to non-existing vertex "
                          << way.destination << std::endl;
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
                std::cout << "[WARNING] Wrong type of arc" << std::endl;
            }

            cairo_move_to(cr.get(),
                          u->second.x - bbox.minx,
                          height - (u->second.y - bbox.miny));
            cairo_line_to(cr.get(),
                          v->second.x - bbox.minx,
                          height - (v->second.y - bbox.miny));
            cairo_stroke(cr.get());
        }
    }

    cairo_surface_write_to_png(surface.get(), png_filename);
}
