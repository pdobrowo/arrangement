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
#include "trianglelistmesh.h"
#include <QVector3D>
#include <GL/gl.h>

// settings
#define DRAW_NORMALS    0
#define DRAW_OUTLINES   0
#define FLAT_SHADING    1

TriangleListMesh::TriangleListMesh(QGLWidget *gl, DecimalTriangleListPtr decimalTriangleList)
    : Mesh(gl),
      m_decimalTriangleList(decimalTriangleList)
{
}

TriangleListMesh::TriangleListMesh(QGLWidget *gl, Mesh_smooth_triangle_list_3_Z_ptr smoothTriangleList)
    : Mesh(gl),
      m_smoothTriangleList(smoothTriangleList)
{
}

void TriangleListMesh::drawMesh()
{
    // FIXME: clean up this mess
    if (m_decimalTriangleList)
        drawDecimalTriangleList();
    else if (m_smoothTriangleList)
        drawSmoothTriangleList();
}

void TriangleListMesh::drawDecimalTriangleList()
{
    // draw faces
    glBegin(GL_TRIANGLES);

    for (DecimalTriangleList::const_iterator iterator = m_decimalTriangleList->begin(); iterator != m_decimalTriangleList->end(); ++iterator)
    {
        const DecimalTriangle &triangle = *iterator;

        QVector3D vertex_a(triangle.vertex(0).x().toDouble(), triangle.vertex(0).y().toDouble(), triangle.vertex(0).z().toDouble());
        QVector3D vertex_b(triangle.vertex(1).x().toDouble(), triangle.vertex(1).y().toDouble(), triangle.vertex(1).z().toDouble());
        QVector3D vertex_c(triangle.vertex(2).x().toDouble(), triangle.vertex(2).y().toDouble(), triangle.vertex(2).z().toDouble());

#if FLAT_SHADING

        QVector3D plane_normal = QVector3D::crossProduct(vertex_b - vertex_a, vertex_c - vertex_a);
        plane_normal /= plane_normal.length();

        glNormal3d(plane_normal.x(), plane_normal.y(), plane_normal.z());

        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());
        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());

#else // FLAT_SHADING

        QVector3D normal_a(triangle.normal_0().x(), triangle.normal_0().y(), triangle.normal_0().z());
        QVector3D normal_b(triangle.normal_1().x(), triangle.normal_1().y(), triangle.normal_1().z());
        QVector3D normal_c(triangle.normal_2().x(), triangle.normal_2().y(), triangle.normal_2().z());

        normal_a /= normal_a.length();
        normal_b /= normal_b.length();
        normal_c /= normal_c.length();

        glNormal3d(normal_a.x(), normal_a.y(), normal_a.z());
        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());

        glNormal3d(normal_b.x(), normal_b.y(), normal_b.z());
        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());

        glNormal3d(normal_c.x(), normal_c.y(), normal_c.z());
        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());

#endif // FLAT_SHADING
    }

    glEnd();

#if DRAW_OUTLINES

    // draw outlines
    glColor3ub(100, 100, 100);

    glBegin(GL_LINES);

    for (DecimalTriangleList::const_iterator iterator = m_decimalTriangleList->begin(); iterator != m_decimalTriangleList->end(); ++iterator)
    {
        const DecimalTriangle &triangle = *iterator;

        QVector3D vertex_a(triangle.vertex(0).x().toDouble(), triangle.vertex(0).y().toDouble(), triangle.vertex(0).z().toDouble());
        QVector3D vertex_b(triangle.vertex(1).x().toDouble(), triangle.vertex(1).y().toDouble(), triangle.vertex(1).z().toDouble());
        QVector3D vertex_c(triangle.vertex(2).x().toDouble(), triangle.vertex(2).y().toDouble(), triangle.vertex(2).z().toDouble());

        QVector3D plane_normal = QVector3D::crossProduct(vertex_b - vertex_a, vertex_c - vertex_a);
        plane_normal /= plane_normal.length();

        glNormal3d(plane_normal.x(), plane_normal.y(), plane_normal.z());

        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());

        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());
        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());

        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());
        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
    }

    glEnd();

#endif // DRAW_OUTLINES

#if DRAW_NORMALS

    // draw normals
    glColor3ub(255, 127, 0);

    glBegin(GL_LINES);

    for (DecimalTriangleList::const_iterator iterator = m_decimalTriangleList->begin(); iterator != m_decimalTriangleList->end(); ++iterator)
    {
        const DecimalTriangle &triangle = *iterator;

        QVector3D vertex_a(triangle.vertex(0).x().toDouble(), triangle.vertex(0).y().toDouble(), triangle.vertex(0).z().toDouble());
        QVector3D vertex_b(triangle.vertex(1).x().toDouble(), triangle.vertex(1).y().toDouble(), triangle.vertex(1).z().toDouble());
        QVector3D vertex_c(triangle.vertex(2).x().toDouble(), triangle.vertex(2).y().toDouble(), triangle.vertex(2).z().toDouble());

        QVector3D normal_a(triangle.normal().x().toDouble(), triangle.normal().y().toDouble(), triangle.normal().z().toDouble());
        QVector3D normal_b(triangle.normal().x().toDouble(), triangle.normal().y().toDouble(), triangle.normal().z().toDouble());
        QVector3D normal_c(triangle.normal().x().toDouble(), triangle.normal().y().toDouble(), triangle.normal().z().toDouble());

        normal_a /= normal_a.length();
        normal_b /= normal_b.length();
        normal_c /= normal_c.length();

        const double SCALE = 0.05;

        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
        glVertex3d(vertex_a.x() + SCALE * normal_a.x(), vertex_a.y() + SCALE * normal_a.y(), vertex_a.z() + SCALE * normal_a.z());

        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());
        glVertex3d(vertex_b.x() + SCALE * normal_b.x(), vertex_b.y() + SCALE * normal_b.y(), vertex_b.z() + SCALE * normal_b.z());

        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());
        glVertex3d(vertex_c.x() + SCALE * normal_c.x(), vertex_c.y() + SCALE * normal_c.y(), vertex_c.z() + SCALE * normal_c.z());
    }

    glEnd();

#endif // DRAW_NORMALS
}

void TriangleListMesh::drawSmoothTriangleList()
{
    // draw faces
    glBegin(GL_TRIANGLES);

    for (Mesh_smooth_triangle_list_3_Z::const_iterator iterator = m_smoothTriangleList->begin(); iterator != m_smoothTriangleList->end(); ++iterator)
    {
        const Mesh_smooth_triangle_3_Z &triangle = *iterator;

        QVector3D vertex_a(triangle.vertex(0).x(), triangle.vertex(0).y(), triangle.vertex(0).z());
        QVector3D vertex_b(triangle.vertex(1).x(), triangle.vertex(1).y(), triangle.vertex(1).z());
        QVector3D vertex_c(triangle.vertex(2).x(), triangle.vertex(2).y(), triangle.vertex(2).z());

#if FLAT_SHADING

        QVector3D plane_normal = QVector3D::crossProduct(vertex_b - vertex_a, vertex_c - vertex_a);
        plane_normal /= plane_normal.length();

        glNormal3d(plane_normal.x(), plane_normal.y(), plane_normal.z());

        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());
        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());

#else // FLAT_SHADING

        QVector3D normal_a(triangle.normal_0().x(), triangle.normal_0().y(), triangle.normal_0().z());
        QVector3D normal_b(triangle.normal_1().x(), triangle.normal_1().y(), triangle.normal_1().z());
        QVector3D normal_c(triangle.normal_2().x(), triangle.normal_2().y(), triangle.normal_2().z());

        normal_a /= normal_a.length();
        normal_b /= normal_b.length();
        normal_c /= normal_c.length();

        glNormal3d(normal_a.x(), normal_a.y(), normal_a.z());
        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());

        glNormal3d(normal_b.x(), normal_b.y(), normal_b.z());
        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());

        glNormal3d(normal_c.x(), normal_c.y(), normal_c.z());
        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());

#endif // FLAT_SHADING
    }

    glEnd();

#if DRAW_OUTLINES

    // draw outlines
    glColor3ub(100, 100, 100);

    glBegin(GL_LINES);

    for (Mesh_smooth_triangle_list_3_Z::const_iterator iterator = m_smoothTriangleList->begin(); iterator != m_smoothTriangleList->end(); ++iterator)
    {
        const Mesh_smooth_triangle_3_Z &triangle = *iterator;

        QVector3D vertex_a(triangle.vertex(0).x(), triangle.vertex(0).y(), triangle.vertex(0).z());
        QVector3D vertex_b(triangle.vertex(1).x(), triangle.vertex(1).y(), triangle.vertex(1).z());
        QVector3D vertex_c(triangle.vertex(2).x(), triangle.vertex(2).y(), triangle.vertex(2).z());

        QVector3D plane_normal = QVector3D::crossProduct(vertex_b - vertex_a, vertex_c - vertex_a);
        plane_normal /= plane_normal.length();

        glNormal3d(plane_normal.x(), plane_normal.y(), plane_normal.z());

        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());

        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());
        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());

        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());
        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
    }

    glEnd();

#endif // DRAW_OUTLINES

#if DRAW_NORMALS

    // draw normals
    glColor3ub(255, 127, 0);

    glBegin(GL_LINES);

    for (Mesh_smooth_triangle_list_3_Z::const_iterator iterator = m_smoothTriangleList->begin(); iterator != m_smoothTriangleList->end(); ++iterator)
    {
        const Mesh_smooth_triangle_3_Z &triangle = *iterator;

        QVector3D vertex_a(triangle.vertex(0).x(), triangle.vertex(0).y(), triangle.vertex(0).z());
        QVector3D vertex_b(triangle.vertex(1).x(), triangle.vertex(1).y(), triangle.vertex(1).z());
        QVector3D vertex_c(triangle.vertex(2).x(), triangle.vertex(2).y(), triangle.vertex(2).z());

        QVector3D normal_a(triangle.normal_0().x(), triangle.normal_0().y(), triangle.normal_0().z());
        QVector3D normal_b(triangle.normal_1().x(), triangle.normal_1().y(), triangle.normal_1().z());
        QVector3D normal_c(triangle.normal_2().x(), triangle.normal_2().y(), triangle.normal_2().z());

        normal_a /= normal_a.length();
        normal_b /= normal_b.length();
        normal_c /= normal_c.length();

        const double SCALE = 0.05;

        glVertex3d(vertex_a.x(), vertex_a.y(), vertex_a.z());
        glVertex3d(vertex_a.x() + SCALE * normal_a.x(), vertex_a.y() + SCALE * normal_a.y(), vertex_a.z() + SCALE * normal_a.z());

        glVertex3d(vertex_b.x(), vertex_b.y(), vertex_b.z());
        glVertex3d(vertex_b.x() + SCALE * normal_b.x(), vertex_b.y() + SCALE * normal_b.y(), vertex_b.z() + SCALE * normal_b.z());

        glVertex3d(vertex_c.x(), vertex_c.y(), vertex_c.z());
        glVertex3d(vertex_c.x() + SCALE * normal_c.x(), vertex_c.y() + SCALE * normal_c.y(), vertex_c.z() + SCALE * normal_c.z());
    }

    glEnd();

#endif // DRAW_NORMALS
}
