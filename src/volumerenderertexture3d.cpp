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
#include "volumerenderertexture3d.h"
#include "scopeddisablelighting.h"
#include "spin3.h"
#include <cs/Benchmark.h>
#include <GL/glew.h>
#include <boost/scoped_array.hpp>
#include <cmath>

namespace // anonymous
{
inline void generateBoxTexCoordAndVertex3f(float x, float y, float z)
{
    glTexCoord3f((x + 1.0) * 0.5,
                 (y + 1.0) * 0.5,
                 (z + 1.0) * 0.5);

    glVertex3f(x, y, z);
}
} // namespace anonymous

VolumeRendererTexture3D::VolumeRendererTexture3D(const VoxelType *voxels, size_t resolution, QGLWidget *gl)
{
    (void)gl;

    boost::scoped_array<unsigned char> data(new unsigned char[resolution * resolution * resolution * 3]);

    size_t index = 0;
    size_t count = resolution * resolution * resolution;

    // scan points
    for (size_t i = 0; i < count; ++i)
    {
        switch (voxels[i])
        {
        case VoxelType_Border:
            data[index++] = 64;
            data[index++] = 64;
            data[index++] = 64;
            break;

        case VoxelType_Imaginary:
            data[index++] = 32;
            data[index++] = 32;
            data[index++] = 32;
            break;

        case VoxelType_Real_Empty:
            data[index++] = 0;
            data[index++] = 255;
            data[index++] = 0;
            break;

        case VoxelType_Real_Full:
            data[index++] = 255;
            data[index++] = 0;
            data[index++] = 0;
            break;

        case VoxelType_Real_Mixed:
            data[index++] = 255;
            data[index++] = 255;
            data[index++] = 0;
            break;
        }
    }

    build3DTexture(data.get(), resolution);
}

void VolumeRendererTexture3D::render()
{
    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

    GLdouble camera[3] = { -(modelview[0] * modelview[12] + modelview[1] * modelview[13] + modelview[2] * modelview[14]),
                           -(modelview[4] * modelview[12] + modelview[5] * modelview[13] + modelview[6] * modelview[14]),
                           -(modelview[8] * modelview[12] + modelview[9] * modelview[13] + modelview[10] * modelview[14]) };

    GLdouble clipPlane[4] = { -modelview[2], -modelview[6], -modelview[10], 0 };
    GLdouble clipPoint[3] = { camera[0] - 3 * modelview[2], camera[1] - 3 * modelview[6], camera[2] - 3 * modelview[10] };

    clipPlane[3] = -(clipPoint[0] * clipPlane[0] + clipPoint[1] * clipPlane[1] + clipPoint[2] * clipPlane[2]);

    // enable cube clip plane
    glClipPlane(GL_CLIP_PLANE0 + 0, clipPlane);
    glEnable(GL_CLIP_PLANE0 + 0);

    // contents
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, m_3dtex);

    glBegin(GL_QUADS);
        // top
        generateBoxTexCoordAndVertex3f(-1.0, -1.0, 1.0);
        generateBoxTexCoordAndVertex3f( 1.0, -1.0, 1.0);
        generateBoxTexCoordAndVertex3f( 1.0,  1.0, 1.0);
        generateBoxTexCoordAndVertex3f(-1.0,  1.0, 1.0);

        // bottom
        generateBoxTexCoordAndVertex3f(-1.0, -1.0, -1.0);
        generateBoxTexCoordAndVertex3f(-1.0,  1.0, -1.0);
        generateBoxTexCoordAndVertex3f( 1.0,  1.0, -1.0);
        generateBoxTexCoordAndVertex3f( 1.0, -1.0, -1.0);

        // left
        generateBoxTexCoordAndVertex3f(-1.0, -1.0, -1.0);
        generateBoxTexCoordAndVertex3f(-1.0, -1.0,  1.0);
        generateBoxTexCoordAndVertex3f(-1.0,  1.0,  1.0);
        generateBoxTexCoordAndVertex3f(-1.0,  1.0, -1.0);

        // right
        generateBoxTexCoordAndVertex3f( 1.0, -1.0, -1.0);
        generateBoxTexCoordAndVertex3f( 1.0,  1.0, -1.0);
        generateBoxTexCoordAndVertex3f( 1.0,  1.0,  1.0);
        generateBoxTexCoordAndVertex3f( 1.0, -1.0,  1.0);

        // front
        generateBoxTexCoordAndVertex3f(-1.0, -1.0,  1.0);
        generateBoxTexCoordAndVertex3f(-1.0, -1.0, -1.0);
        generateBoxTexCoordAndVertex3f( 1.0, -1.0, -1.0);
        generateBoxTexCoordAndVertex3f( 1.0, -1.0,  1.0);

        // back
        generateBoxTexCoordAndVertex3f( 1.0,  1.0,  1.0);
        generateBoxTexCoordAndVertex3f( 1.0,  1.0, -1.0);
        generateBoxTexCoordAndVertex3f(-1.0,  1.0, -1.0);
        generateBoxTexCoordAndVertex3f(-1.0,  1.0,  1.0);
    glEnd();

    glDisable(GL_TEXTURE_3D);

    // disable cube clip plane
    glDisable(GL_CLIP_PLANE0 + 0);

    // draw box frames
    if (0)
    {
        ScopedDisableLighting noLighting;
        glColor3ub(128, 128, 128);

        glBegin(GL_LINES);
            glVertex3f(-1.0, -1.0, -1.0); glVertex3f( 1.0, -1.0, -1.0);
            glVertex3f(-1.0, -1.0,  1.0); glVertex3f( 1.0, -1.0,  1.0);
            glVertex3f(-1.0,  1.0, -1.0); glVertex3f( 1.0,  1.0, -1.0);
            glVertex3f(-1.0,  1.0,  1.0); glVertex3f( 1.0,  1.0,  1.0);

            glVertex3f(-1.0, -1.0, -1.0); glVertex3f(-1.0,  1.0, -1.0);
            glVertex3f( 1.0, -1.0, -1.0); glVertex3f( 1.0,  1.0, -1.0);
            glVertex3f(-1.0, -1.0,  1.0); glVertex3f(-1.0,  1.0,  1.0);
            glVertex3f( 1.0, -1.0,  1.0); glVertex3f( 1.0,  1.0,  1.0);

            glVertex3f(-1.0, -1.0, -1.0); glVertex3f(-1.0, -1.0,  1.0);
            glVertex3f( 1.0, -1.0, -1.0); glVertex3f( 1.0, -1.0,  1.0);
            glVertex3f(-1.0,  1.0, -1.0); glVertex3f(-1.0,  1.0,  1.0);
            glVertex3f( 1.0,  1.0, -1.0); glVertex3f( 1.0,  1.0,  1.0);
        glEnd();
    }

    // setup cap planes
    GLdouble capClipPlanes[6][4] =
    {
        {  1,  0,  0, 1 },
        { -1,  0,  0, 1 },
        {  0,  1,  0, 1 },
        {  0, -1,  0, 1 },
        {  0,  0,  1, 1 },
        {  0,  0, -1, 1 }
    };

    // enable cube clip plane
    for (int k = 0; k < 6; ++k)
    {
        glClipPlane(GL_CLIP_PLANE0 + k, capClipPlanes[k]);
        glEnable(GL_CLIP_PLANE0 + k);
    }

    // cap plane: transromed 2x2 z positive oriented quad
    Spin3 spin;
    spin.fromVectorAlign(0.0, 0.0, -1.0, clipPlane[0], clipPlane[1], clipPlane[2]);

    double va[3], vb[3], vc[3], vd[3];
    spin.transformVector(-2.0, -2.0, clipPlane[3], va + 0, va + 1, va + 2);
    spin.transformVector( 2.0, -2.0, clipPlane[3], vb + 0, vb + 1, vb + 2);
    spin.transformVector( 2.0,  2.0, clipPlane[3], vc + 0, vc + 1, vc + 2);
    spin.transformVector(-2.0,  2.0, clipPlane[3], vd + 0, vd + 1, vd + 2);

    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, m_3dtex);

    glBegin(GL_QUADS);
        generateBoxTexCoordAndVertex3f(va[0], va[1], va[2]);
        generateBoxTexCoordAndVertex3f(vb[0], vb[1], vb[2]);
        generateBoxTexCoordAndVertex3f(vc[0], vc[1], vc[2]);
        generateBoxTexCoordAndVertex3f(vd[0], vd[1], vd[2]);
    glEnd();

    glDisable(GL_TEXTURE_3D);

    // disable cube clip plane
    for (int k = 0; k < 6; ++k)
        glDisable(GL_CLIP_PLANE0 + k);
}

void VolumeRendererTexture3D::build3DTexture(const unsigned char *texels, size_t resolution)
{
    glGenTextures(1, &m_3dtex);
    glBindTexture(GL_TEXTURE_3D, m_3dtex);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, resolution, resolution, resolution, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);
}
