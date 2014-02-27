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
#include "variantpredicate.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

// force kernel instantiation
namespace
{
    static Kernel_Z g_kernel;
    static Kernel_R r_kernel;
    inline void dummyKernelProcZ() { (void)g_kernel; }
    inline void dummyKernelProcR() { (void)r_kernel; }
}

void clearAndParse(const std::string &str, CGAL::Bigint &out)
{
    std::string clearStr;

    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
        if (*i != ' ' && *i != ';')
            clearStr += *i;

    LiDIA::string_to_bigint(clearStr.c_str(), out);
}

void clearAndParse(const std::string &str, double &out)
{
    std::string clearStr;

    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
        if (*i != ' ' && *i != ';')
            clearStr += *i;

    out = boost::lexical_cast<double>(clearStr);
}

Vector_3_Z toVector_3(const std::string &text, bool *ok)
{
    std::vector<std::string> parts;
    boost::split(parts, text, boost::is_any_of(";"));

    if (parts.size() != 3)
    {
        if (ok)
            *ok = false;

        return Vector_3_Z(0, 0, 0);
    }

    CGAL::Bigint x, y, z;

    clearAndParse(parts[0], x);
    clearAndParse(parts[1], y);
    clearAndParse(parts[2], z);

    if (ok)
        *ok = true;

    return Vector_3_Z(x, y, z);
}

Mesh_point_3_Z toMesh_point_3(const std::string &text, bool *ok)
{
    std::vector<std::string> parts;
    boost::split(parts, text, boost::is_any_of(";"));

    if (parts.size() != 3)
    {
        if (ok)
            *ok = false;

        return Mesh_point_3_Z(0, 0, 0);
    }

    double x, y, z;

    clearAndParse(parts[0], x);
    clearAndParse(parts[1], y);
    clearAndParse(parts[2], z);

    if (ok)
        *ok = true;

    return Mesh_point_3_Z(x, y, z);
}

Plane_3_Z toPlane_3(const std::string &text, bool *ok)
{
    std::vector<std::string> parts;
    boost::split(parts, text, boost::is_any_of(";"));

    if (parts.size() != 4)
    {
        if (ok)
            *ok = false;

        return Plane_3_Z();
    }

    CGAL::Bigint a, b, c, d;

    clearAndParse(parts[0], a);
    clearAndParse(parts[1], b);
    clearAndParse(parts[2], c);
    clearAndParse(parts[3], d);

    if (ok)
        *ok = true;

    return Plane_3_Z(a, b, c, d);
}

CGAL::Bigint toNumber(const std::string &text, bool *ok)
{
    CGAL::Bigint result;
    clearAndParse(text, result);
    if (ok) *ok = true;
    return result;
}
