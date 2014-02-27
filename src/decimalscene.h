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
#ifndef DECIMALSCENE_H
#define DECIMALSCENE_H

#include "qdecimal.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <cassert>

class DecimalVector
{
public:
    DecimalVector()
        : m_x(0), m_y(0), m_z(0)
    {
    }

    DecimalVector(const QDecimal &x, const QDecimal &y, const QDecimal &z)
        : m_x(x), m_y(y), m_z(z)
    {
    }

    QDecimal            squaredLength() const
    {
        return m_x * m_x + m_y * m_y + m_z * m_z;
    }

    const QDecimal &    x() const
    {
        return m_x;
    }

    const QDecimal &    y() const
    {
        return m_y;
    }

    const QDecimal &    z() const
    {
        return m_z;
    }

    DecimalVector &operator *=(const QDecimal &other)
    {
        m_x *= other;
        m_y *= other;
        m_z *= other;
        return *this;
    }

private:
    QDecimal m_x, m_y, m_z;
};

inline DecimalVector operator -(const DecimalVector &left, const DecimalVector &right)
{
    return DecimalVector(left.x() - right.x(), left.y() - right.y(), left.z() - right.z());
}

inline DecimalVector operator *(const DecimalVector &left, const QDecimal &right)
{
    return DecimalVector(left.x() * right, left.y() * right, left.z() * right);
}

inline DecimalVector operator /(const DecimalVector &left, const QDecimal &right)
{
    return DecimalVector(left.x() / right, left.y() / right, left.z() / right);
}

inline DecimalVector operator *(const QDecimal &left, const DecimalVector &right)
{
    return DecimalVector(left * right.x(), left * right.y(), left * right.z());
}

inline QDecimal operator *(const DecimalVector &left, const DecimalVector &right)
{
    return left.x() * right.x() + left.y() * right.y() + left.z() * right.z();
}

inline DecimalVector cross(const DecimalVector &left, const DecimalVector &right)
{
    return DecimalVector(left.y() * right.z() - left.z() * right.y(),
                         left.z() * right.x() - left.x() * right.z(),
                         left.x() * right.y() - left.y() * right.x());
}

inline QDecimal dot(const DecimalVector &left, const DecimalVector &right)
{
    return left * right;
}

class DecimalFace
{
public:
    DecimalFace()
        : m_vertexA(0), m_vertexB(0), m_vertexC(0)
    {
    }

    DecimalFace(size_t vertexA, size_t vertexB, size_t vertexC)
        : m_vertexA(vertexA), m_vertexB(vertexB), m_vertexC(vertexC)
    {
    }

    size_t vertex(int i) const
    {
        switch (i)
        {
        case 0: return m_vertexA;
        case 1: return m_vertexB;
        case 2: return m_vertexC;
        default: assert(0); return 0;
        }
    }

private:
    size_t m_vertexA, m_vertexB, m_vertexC;
};

typedef std::vector<DecimalVector>  DecimalVectorList;
typedef std::vector<DecimalFace>    DecimalFaceList;

class DecimalTriangle
{
public:
    DecimalTriangle()
    {
        m_vertexA = m_vertexB = m_vertexC = DecimalVector(0, 0, 0);
        m_normal = cross(m_vertexB - m_vertexA, m_vertexC - m_vertexA);
    }

    DecimalTriangle(const DecimalVector &vertexA, const DecimalVector &vertexB, const DecimalVector &vertexC)
    {
        m_vertexA = vertexA;
        m_vertexB = vertexB;
        m_vertexC = vertexC;
        m_normal = cross(m_vertexB - m_vertexA, m_vertexC - m_vertexA);
    }

    const DecimalVector &   vertex(int i) const
    {
        switch (i)
        {
        case 0: return m_vertexA;
        case 1: return m_vertexB;
        case 2: return m_vertexC;
        default: assert(0); return m_vertexA;
        }
    }

    const DecimalVector &   normal() const
    {
        return m_normal;
    }

private:
    DecimalVector   m_vertexA, m_vertexB, m_vertexC;
    DecimalVector   m_normal;
};

typedef std::vector<DecimalTriangle>            DecimalTriangleList;
typedef boost::shared_ptr<DecimalTriangleList>  DecimalTriangleListPtr;

class DecimalBall
{
public:
    DecimalBall()
    {
        m_center = DecimalVector(0, 0, 0);
        m_radius = 0;
    }

    DecimalBall(const DecimalVector &center_, const QDecimal &radius_)
    {
        m_center = center_;
        m_radius = radius_;
    }

    const DecimalVector &   center() const
    {
        return m_center;
    }

    const QDecimal &        radius() const
    {
        return m_radius;
    }

    void                    scale(const QDecimal &scale)
    {
        m_center *= scale;
        m_radius *= scale;
    }

private:
    DecimalVector   m_center;
    QDecimal        m_radius;
};

typedef std::vector<DecimalBall>            DecimalBallList;
typedef boost::shared_ptr<DecimalBallList>  DecimalBallListPtr;

#endif // DECIMALSCENE_H
