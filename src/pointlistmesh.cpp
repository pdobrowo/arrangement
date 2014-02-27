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
#include "pointlistmesh.h"
#include <GL/gl.h>

PointListMesh::PointListMesh(QGLWidget *gl, Point *begin, Point *end)
    : Mesh(gl),
      m_points(begin, end)
{
}

void PointListMesh::drawMesh()
{
    // draw points
    glBegin(GL_POINTS);

    for (std::vector<Point>::const_iterator iterator = m_points.begin(); iterator != m_points.end(); ++iterator)
    {
        glColor3f(iterator->r, iterator->g, iterator->b);
        glVertex3f(iterator->x, iterator->y, iterator->z);
    }

    glEnd();
}
