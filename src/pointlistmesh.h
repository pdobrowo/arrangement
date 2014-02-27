/**
 * Copyright (C) 2009-2013  Przemysław Dobrowolski
 *
 * This file is part of the Configuration Space Library (libcs), a library
 * for creating configuration spaces of various motion planning problems.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef POINTLISTMESH_H
#define POINTLISTMESH_H

#include "mesh.h"
#include <vector>

class PointListMesh
    : public Mesh
{
public:
    struct Point
    {
        double x, y, z;
        float r, g, b;

        Point()
            : x(0.0), y(0.0), z(0.0), r(0.0f), g(0.0f), b(0.0f)
        {
        }

        Point(double x_, double y_, double z_,
                  float r_, float g_, float b_)
            : x(x_), y(y_), z(z_), r(r_), g(g_), b(b_)
        {
        }
    };

    PointListMesh(QGLWidget *gl,
                  Point *begin, Point *end);

private:
    std::vector<Point> m_points;

protected:
    virtual void drawMesh();
};

#endif // POINTLISTMESH_H
