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
#ifndef SAMPLEDROUTE_H
#define SAMPLEDROUTE_H

#include "configurationspace.h"
#include "genericrouter.h"
#include "polyconemesh.h"
#include <boost/scoped_ptr.hpp>

class QGLWidget;

class SampledRoute
    : public ConfigurationSpace
{
public:
    static const int NUMBER_OF_SAMPLES = 100;

    SampledRoute(RoutePtr route, QGLWidget *gl)
        : ConfigurationSpace(gl)
    {
        // evaluate curve
        Qsic_spin_list_3_Z_ptr spinList(new Qsic_spin_list_3_Z());

        for (int i = 0; i <= NUMBER_OF_SAMPLES; ++i)
        {
            double time = double(i) / double(NUMBER_OF_SAMPLES);
            QQuaternion quaternion = route->evaluate(time);
            typename Qsic_spin_3_Z::Spin_3 spin(-quaternion.z() /*e12*/, -quaternion.x() /*e23*/, -quaternion.y() /*e31*/, quaternion.scalar() /*1*/);
            spinList->push_back(spin);
        }

        // create poly cones
        PolyConeMeshPtr polyConeMesh(new PolyConeMesh(m_gl, spinList, 0.03, 12));

        // store poly cones
        m_polyConeMesh = polyConeMesh;
    }

    virtual void render()
    {
        setColorGL(QColor(51, 147, 41));
        m_polyConeMesh->render();
    }

    virtual bool needsLighting() const
    {
        return true;
    }

    virtual bool needsShaderLighting() const
    {
        return true;
    }

private:
    PolyConeMeshPtr       m_polyConeMesh;
};

typedef boost::shared_ptr<SampledRoute> SampledRoutePtr;

#endif // SAMPLEDROUTE_H
