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
#ifndef CONFIGURATIONOBJECT_H
#define CONFIGURATIONOBJECT_H

#include "kernel.h"
#include <QString>
#include "configurationspace.h"
#include "rasterconfigurationspace.h"
#include "cellconfigurationspace.h"
#include "exactconfigurationspace.h"
#include "sampledroute.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <utility>

class QWidget;

class ConfigurationObject;
typedef boost::shared_ptr<ConfigurationObject> ConfigurationObjectPtr;

class ConfigurationObject
    : private boost::noncopyable
{
public:
    enum Type
    {
        Type_RasterConfigurationSpace,
        Type_CellConfigurationSpace,
        Type_ExactConfigurationSpace,
        Type_Route
    };

    explicit                        ConfigurationObject(RasterConfigurationSpacePtr rasterConfigurationSpace);
    explicit                        ConfigurationObject(CellConfigurationSpacePtr cellConfigurationSpace);
    explicit                        ConfigurationObject(ExactConfigurationSpacePtr exactConfigurationSpace);
    explicit                        ConfigurationObject(RoutePtr route, SampledRoutePtr sampledRoute);

    Type                            type() const;
    QString                         typeString() const;

    RasterConfigurationSpacePtr     rasterConfigurationSpace() const;
    CellConfigurationSpacePtr       cellConfigurationSpace() const;
    ExactConfigurationSpacePtr      exactConfigurationSpace() const;
    RoutePtr                        route() const;
    SampledRoutePtr                 sampledRoute() const;

    void                            setVisible(bool visible);
    bool                            isVisible() const;

    RoutePtr                        findRoute(const QQuaternion &begin, const QQuaternion &end) const;

    static ConfigurationObjectPtr   loadFromFile(const QString &fileName, QWidget *parent, QGLWidget *gl);
    void                            saveToFile(const QString &fileName, QWidget *parent);

private:
    Type                            m_type;

    RasterConfigurationSpacePtr     m_rasterConfigurationSpace;
    CellConfigurationSpacePtr       m_cellConfigurationSpace;
    ExactConfigurationSpacePtr      m_exactConfigurationSpace;
    RoutePtr                        m_route;
    SampledRoutePtr                 m_sampledRoute;

    bool                            m_visible;
};

#endif // CONFIGURATIONOBJECT_H
