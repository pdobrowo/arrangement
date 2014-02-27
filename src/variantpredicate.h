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
#ifndef PREDICATE_H
#define PREDICATE_H

#include "kernel.h"
#include <cs/Loader_sphere_tree.h>
#include <string>

typedef CS::Loader_sphere_tree<Kernel_R>             SphereTreeLoader;

struct VariantPredicate
{
    enum Type
    {
        None,
        Halfspace,
        Screw,
        Generic
    };

    Type type;

    Predicate_H_3_Z h3;
    Predicate_S_3_Z s3;
    Predicate_G_3_Z g3;

    VariantPredicate()
        : type(None)
    {
    }

    VariantPredicate(const Predicate_H_3_Z &p)
        : type(Halfspace),
          h3(p)
    {
    }

    VariantPredicate(const Predicate_S_3_Z &p)
        : type(Screw),
          s3(p)
    {
    }

    VariantPredicate(const Predicate_G_3_Z &p)
        : type(Generic),
          g3(p)
    {
    }
};

// TODO: use boost::variant<Predicate>
Vector_3_Z      toVector_3(const std::string &text, bool *ok = 0);
Mesh_point_3_Z  toMesh_point_3(const std::string &text, bool *ok = 0);
Plane_3_Z       toPlane_3(const std::string &text, bool *ok = 0);
CGAL::Bigint    toNumber(const std::string &text, bool *ok = 0);

#endif // PREDICATE_H
