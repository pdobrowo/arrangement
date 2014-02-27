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
#ifndef VOLUMERENDERERMARCHINGCUBES_H
#define VOLUMERENDERERMARCHINGCUBES_H

#include "volumerenderer.h"
#include "trianglelistmesh.h"
#include <cstdlib>

class vtkPolyDataAlgorithm;
class QGLWidget;

class VolumeRendererGaussianSplatter
    : public VolumeRenderer
{
public:
    VolumeRendererGaussianSplatter(const VoxelType *voxels, size_t resolution, QGLWidget *gl);
    VolumeRendererGaussianSplatter(const Voxel *begin, const Voxel *end, QGLWidget *gl);

    virtual void render();

private:
    TriangleListMeshPtr m_triangleListMeshFull;
    TriangleListMeshPtr m_triangleListMeshMixed;

    TriangleListMeshPtr meshVoxels(vtkPolyDataAlgorithm *dataSource, QGLWidget *gl);
};

#endif // VOLUMERENDERERMARCHINGCUBES_H
