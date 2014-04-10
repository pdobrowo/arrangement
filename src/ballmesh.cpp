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
#include "ballmesh.h"
#include <GL/glu.h>

BallMesh::BallMesh(QGLWidget *gl, double radius, int slices, int stacks)
    : Mesh(gl),
      m_radius(radius),
      m_slices(slices),
      m_stacks(stacks)
{
}

void BallMesh::renderTranslated(double x, double y, double z)
{
    glPushMatrix();
        glTranslated(x, y, z);
        Mesh::render();
    glPopMatrix();
}

void BallMesh::drawMesh()
{
    GLUquadric *quadric;

    // quadric
    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // draw
    gluSphere(quadric, m_radius, m_slices, m_stacks);

    // done
    gluDeleteQuadric(quadric);
}
