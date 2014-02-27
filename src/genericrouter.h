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
#ifndef GENERICROUTER_H
#define GENERICROUTER_H

#include "configurationspace.h"
#include <QQuaternion>

template<class Configuration_>
class GenericRoute
    : public Route
{
    typedef Configuration_                  Configuration;
    typedef typename Configuration::Sample  Sample;
    typedef typename Configuration::Route   ConfigurationRoute;

public:
    GenericRoute(const ConfigurationRoute &route)
        : m_route(route)
    {
    }

    virtual QQuaternion evaluate(double t) const
    {
        return sampleToQuaternion(m_route.evaluate(t));
    }

private:
    ConfigurationRoute  m_route;

    QQuaternion         sampleToQuaternion(const Sample &sample) const
    {
        return QQuaternion(sample.s0() /*1*/, -sample.s23() /*i*/, -sample.s31() /*j*/, -sample.s12() /*k*/);
    }
};

template<class Configuration_>
class GenericRouter
    : public Router
{
    typedef Configuration_                  Configuration;
    typedef typename Configuration::Sample  Sample;
    typedef typename Configuration::Route   ConfigurationRoute;

public:
    Configuration &         configuration()
    {
        return m_configuration;
    }

    const Configuration &   configuration() const
    {
        return m_configuration;
    }

    virtual RoutePtr        findRoute(const QQuaternion &begin, const QQuaternion &end)
    {
        ConfigurationRoute route = m_configuration.find_route(quaternionToSample(begin),
                                                              quaternionToSample(end));

        if (!route.is_valid())
            return RoutePtr();

        return RoutePtr(new GenericRoute<Configuration>(route));
    }

private:
    Configuration           m_configuration;

    Sample                  quaternionToSample(const QQuaternion &quaternion) const
    {
        return Sample(-quaternion.z() /*e12*/, -quaternion.x() /*e23*/, -quaternion.y() /*e31*/, quaternion.scalar() /*1*/);
    }
};

#endif // GENERICROUTER_H
