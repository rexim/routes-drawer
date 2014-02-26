#include <istream>
#include <fstream>
#include <iostream>

#include "./graph.hpp"
#include "./binary_parser.hpp"
#include "./graph_dumper.hpp"

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
        std::cerr << "Usage: ./routing-drawer routing.bin map.png" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream fin(argv[1], std::ifstream::in | std::ifstream::binary);
    auto header = read_header(&fin);
    print_header(&fin, header);

    AdjacencyList adjacency_list;
    read_adjacency_list(&fin, &adjacency_list);

    std::cout << "Size of Adjacency List: "
              << adjacency_list.size() << std::endl;

    VertexList vertex_list;
    read_vertex_list(&fin, &vertex_list);

    std::cout << "Size of Vertex List: " << vertex_list.size() << std::endl;

    dump_graph_to_png_file(adjacency_list, vertex_list, argv[2]);

    return EXIT_SUCCESS;
}
