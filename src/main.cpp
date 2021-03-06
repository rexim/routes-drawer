#include <istream>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "./graph.hpp"
#include "./binary_parser.hpp"
#include "./graph_dumper.hpp"

#define SCALE_RATIO 0.01

void print_header(std::istream *is, const RoutingHeader &header)
{
    std::cout << "Transfer Time: " << header.transfer_time << std::endl;
    std::cout << "Pedestrian Speed: " << header.pedestrian_speed << std::endl;
    std::cout << "Convertion Ratio: " << header.convertion_ratio << std::endl;
    std::cout << "Max Distance: " << header.max_distance << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: ./routes-drawer <binary file> <map.png>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream fin(argv[1], std::ifstream::in | std::ifstream::binary);

    if (!fin) {
        throw std::runtime_error("Cannot open file: " + std::string(argv[1]));
    }

    auto header = read_header(&fin);
    print_header(&fin, header);

    AdjacencyList adjacency_list;
    read_adjacency_list(&fin, &adjacency_list);

    std::cout << "Size of Adjacency List: "
              << adjacency_list.size() << std::endl;

    VertexList vertex_list;
    read_vertex_list(&fin, &vertex_list, SCALE_RATIO);

    std::cout << "Size of Vertex List: " << vertex_list.size() << std::endl;

    dump_graph_to_png_file(adjacency_list, vertex_list, argv[2]);

    return EXIT_SUCCESS;
}
