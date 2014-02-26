#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

#include "./graph.hpp"

struct AABB
{
    AABB(double minx, double miny,
         double maxx, double maxy):
        minx(minx), miny(miny),
        maxx(maxx), maxy(maxy)
    {}

    double minx;
    double miny;
    double maxx;
    double maxy;
};

AABB get_aabb(const VertexList &vertex_list);
double distance(const Vertex &p1, const Vertex &p2);

#endif  // GEOMETRY_HPP_
