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
#ifndef CONFIGURATIONSPACE_H
#define CONFIGURATIONSPACE_H

#include "mesh.h"
#include <boost/shared_ptr.hpp>
#include <QQuaternion>

class QGLWidget;

class Route
{
public:
    virtual ~Route() {}

    virtual QQuaternion evaluate(double t) const = 0;
};

typedef boost::shared_ptr<Route> RoutePtr;

class Router
{
public:
    virtual ~Router() {}

    virtual RoutePtr    findRoute(const QQuaternion &begin, const QQuaternion &end) = 0;
};

typedef boost::shared_ptr<Router> RouterPtr;

// this is an abstract meshed configuration space
// in fact, this is simply a mesh for render view but we want abstraction
class ConfigurationSpace
{
public:
    ConfigurationSpace(QGLWidget *gl)
        : m_gl(gl)
    {
    }

    virtual ~ConfigurationSpace()
    {
    }

    virtual void render() = 0;

    // router
    RouterPtr router() const
    {
        return m_router;
    }

    // lighting
    virtual bool needsLighting() const
    {
        return false;
    }

    virtual bool needsShaderLighting() const
    {
        return false;
    }

protected:
    QGLWidget * m_gl;
    RouterPtr   m_router;
};

typedef boost::shared_ptr<ConfigurationSpace> ConfigurationSpacePtr;

#endif // CONFIGURATIONSPACE_H
