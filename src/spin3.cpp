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
#include "spin3.h"
#include <cmath>

Spin3::Spin3()
    : s12(0.0),
      s23(0.0),
      s31(0.0),
      s0(1.0)
{
}

void Spin3::fromVectorAlign(double x0, double y0, double z0, double x1, double y1, double z1)
{
    double u = x0 + x1;
    double v = y0 + y1;
    double w = z0 + z1;

    double m = std::sqrt(u * u + v * v + w * w);

    u /= m;
    v /= m;
    w /= m;

    double a = x0;
    double b = y0;
    double c = z0;

    double n = std::sqrt(a * a + b * b + c * c);

    a /= n;
    b /= n;
    c /= n;

    s0 = u * a + v * b + w * c;
    s12 = u * b - v * a;
    s23 = v * c - w * b;
    s31 = w * a - u * c;
}

void Spin3::transformVector(double x, double y, double z, double *outX, double *outY, double *outZ)
{
    *outX = (s0*s0 - s12*s12 + s23*s23 - s31*s31)*x + 2*(y*(s0*s12 + s23*s31) + z*(s12*s23 - s0*s31));
    *outY = (s0*s0 - s12*s12 - s23*s23 + s31*s31)*y + 2*(x*(s23*s31 - s0*s12) + z*(s12*s31 + s0*s23));
    *outZ = (s0*s0 + s12*s12 - s23*s23 - s31*s31)*z + 2*(x*(s12*s23 + s0*s31) + y*(s12*s31 - s0*s23));
}
