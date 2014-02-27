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
#include "polyconemesh.h"
#include <GL/gle.h>
#include <GL/gl.h>
#include <boost/scoped_array.hpp>

PolyConeMesh::PolyConeMesh(QGLWidget *gl, Qsic_spin_list_3_Z_ptr spinList, double radius, int sides)
    : Mesh(gl),
      m_spinList(spinList),
      m_radius(radius),
      m_sides(sides)
{
}

void PolyConeMesh::drawMesh()
{
    // draw qsic
    boost::scoped_array<gleDouble [3]> points(new gleDouble[m_spinList->size()][3]);
    boost::scoped_array<gleDouble> radiuses(new gleDouble[m_spinList->size()]);
    size_t index = 0;

    for (Qsic_spin_list_3_Z::const_iterator iterator = m_spinList->begin(); iterator != m_spinList->end(); ++iterator)
    {
        points[index][0] = iterator->s12();
        points[index][1] = iterator->s23();
        points[index][2] = iterator->s31();
        radiuses[index] = m_radius * ((iterator->s0() + 1.0) * 0.5 * 0.75 + 0.25);   // [-1; 1] -> [25% QSIC_SIZE; 100 % QSIC_SIZE]
        ++index;
    }

    gleSetNumSides(m_sides);
    gleSetJoinStyle(TUBE_JN_ANGLE | TUBE_NORM_EDGE);

    glePolyCone(static_cast<int>(m_spinList->size()),
                points.get(),
                NULL,
                radiuses.get());
}
