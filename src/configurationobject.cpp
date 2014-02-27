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
#include "configurationobject.h"
#include "sceneloader.h"
#include "rasterconfigurationspace.h"
#include "cellconfigurationspace.h"
#include "exactconfigurationspace.h"
#include <cs/Loader_sphere_tree.h>
#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QInputDialog>

ConfigurationObject::Type ConfigurationObject::type() const
{
    return m_type;
}

QString ConfigurationObject::typeString() const
{
    switch (m_type)
    {
        case Type_RasterConfigurationSpace: return "RasterConfigurationSpace";
        case Type_CellConfigurationSpace:   return "CellConfigurationSpace";
        case Type_ExactConfigurationSpace:  return "ExactConfigurationSpace";
        case Type_Route:                    return "SampledRoute";
        default:                            return "<unknown>";
    }
}

RasterConfigurationSpacePtr ConfigurationObject::rasterConfigurationSpace() const
{
    return m_rasterConfigurationSpace;
}

CellConfigurationSpacePtr ConfigurationObject::cellConfigurationSpace() const
{
    return m_cellConfigurationSpace;
}

ExactConfigurationSpacePtr ConfigurationObject::exactConfigurationSpace() const
{
    return m_exactConfigurationSpace;
}

RoutePtr ConfigurationObject::route() const
{
    return m_route;
}

SampledRoutePtr ConfigurationObject::sampledRoute() const
{
    return m_sampledRoute;
}

void ConfigurationObject::setVisible(bool visible)
{
    m_visible = visible;
}

bool ConfigurationObject::isVisible() const
{
    return m_visible;
}

RoutePtr ConfigurationObject::findRoute(const QQuaternion &begin, const QQuaternion &end) const
{
    // get a configuration space router
    RouterPtr router;

    switch (m_type)
    {
    case Type_RasterConfigurationSpace:
        router = rasterConfigurationSpace()->router();
        break;

    case Type_CellConfigurationSpace:
        router = cellConfigurationSpace()->router();
        break;

    case Type_ExactConfigurationSpace:
        router = exactConfigurationSpace()->router();
        break;

    case Type_Route:
        break;
    }

    // is there a router available?
    if (!router)
        return RoutePtr();

    // execute router and search for a route
    return router->findRoute(begin, end);
}

ConfigurationObjectPtr ConfigurationObject::loadFromFile(const QString &fileName, QWidget *parent, QGLWidget *gl)
{
    Q_UNUSED(parent);

    // create data stream
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
        return ConfigurationObjectPtr();

    QDataStream stream(&file);

    // save type info
    uint rawType;
    stream >> rawType;

    if (stream.status() == QDataStream::Ok)
    {
        ConfigurationObject::Type type = static_cast<ConfigurationObject::Type >(rawType);

        // save data
        switch (type)
        {
        case Type_RasterConfigurationSpace:
            try
            {
                RasterConfigurationSpacePtr configurationSpace(new RasterConfigurationSpace(stream, VolumeRendererType_GaussianSplatter, gl));
                return ConfigurationObjectPtr(new ConfigurationObject(configurationSpace));
            }
            catch (const std::runtime_error &e)
            {
                // ignore error here
            }
            break;

        case Type_CellConfigurationSpace:
            try
            {
                CellConfigurationSpacePtr configurationSpace(new CellConfigurationSpace(stream, gl));
                return ConfigurationObjectPtr(new ConfigurationObject(configurationSpace));
            }
            catch (const std::runtime_error &e)
            {
                // ignore error here
            }
            break;

        case Type_ExactConfigurationSpace:
            try
            {
                ExactConfigurationSpacePtr configurationSpace(new ExactConfigurationSpace(stream, gl));
                return ConfigurationObjectPtr(new ConfigurationObject(configurationSpace));
            }
            catch (const std::runtime_error &e)
            {
                // ignore error here
            }
            break;

        case Type_Route:
            break;
        }
    }

    QMessageBox::warning(parent, QObject::tr("Load configuration object"), QObject::tr("Failed to load configuration object!"), QMessageBox::Ok);
    return ConfigurationObjectPtr();
}

void ConfigurationObject::saveToFile(const QString &fileName, QWidget *parent)
{
    // create data stream
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly))
        return;

    QDataStream stream(&file);
    bool failed = false;

    // save type info
    stream << static_cast<uint>(m_type);

    if (stream.status() != QDataStream::Ok)
    {
        failed = true;
    }
    else
    {
        // save data
        switch (m_type)
        {
        case Type_RasterConfigurationSpace:
            if (!m_rasterConfigurationSpace->saveToStream(stream))
                failed = true;
            break;

        case Type_CellConfigurationSpace:
            if (!m_cellConfigurationSpace->saveToStream(stream))
                failed = true;
            break;

        case Type_ExactConfigurationSpace:
            if (!m_exactConfigurationSpace->saveToStream(stream))
                failed = true;
            break;

        case Type_Route:
            break;
        }
    }

    if (failed)
        QMessageBox::warning(parent, QObject::tr("Save configuration object"), QObject::tr("Failed to save configuration object!"), QMessageBox::Ok);
}

ConfigurationObject::ConfigurationObject(RasterConfigurationSpacePtr rasterConfigurationSpace)
    : m_type(Type_RasterConfigurationSpace),
      m_rasterConfigurationSpace(rasterConfigurationSpace),
      m_visible(true)
{
}

ConfigurationObject::ConfigurationObject(CellConfigurationSpacePtr cellConfigurationSpace)
    : m_type(Type_CellConfigurationSpace),
      m_cellConfigurationSpace(cellConfigurationSpace),
      m_visible(true)
{
}

ConfigurationObject::ConfigurationObject(ExactConfigurationSpacePtr exactConfigurationSpace)
    : m_type(Type_ExactConfigurationSpace),
      m_exactConfigurationSpace(exactConfigurationSpace),
      m_visible(true)
{
}

ConfigurationObject::ConfigurationObject(RoutePtr route, SampledRoutePtr sampledRoute)
    : m_type(Type_Route),
      m_route(route),
      m_sampledRoute(sampledRoute),
      m_visible(true)
{
}
