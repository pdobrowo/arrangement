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
#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

enum VoxelType
{
    VoxelType_Real_Empty,
    VoxelType_Real_Full,
    VoxelType_Real_Mixed,
    VoxelType_Imaginary,
    VoxelType_Border
};

struct Voxel
{
    Voxel()
        : type(VoxelType_Border),
          x(0),
          y(0),
          z(0)
    {
    }

    Voxel(VoxelType type_,
          double x_,
          double y_,
          double z_)
        : type(type_),
          x(x_),
          y(y_),
          z(z_)
    {
    }

    VoxelType type;
    double x, y, z;
};

class VolumeRenderer
{
public:
    virtual ~VolumeRenderer() {}

    virtual void render() = 0;
};

#endif // VOLUMERENDERER_H
