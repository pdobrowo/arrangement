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
#include "sceneobject.h"
#include "sceneloader.h"
#include "spheretreeloader.h"
#include <cs/Loader_sphere_tree.h>
#include <QMessageBox>
#include <QInputDialog>

namespace // anonymous
{
DecimalTriangleListPtr sceneLoaderObjectToTriangleList(SceneLoaderPtr object)
{
    DecimalTriangleListPtr triangleList(new DecimalTriangleList());

    foreach (const DecimalFace &face, object->faces())
    {
        DecimalVector a = object->vertices()[face.vertex(0)];
        DecimalVector b = object->vertices()[face.vertex(1)];
        DecimalVector c = object->vertices()[face.vertex(2)];

        triangleList->push_back(DecimalTriangle(a, b, c));
    }

    return triangleList;
}
} // namespace anonymous

SceneObject::Type SceneObject::type() const
{
    return m_type;
}

QString SceneObject::typeString() const
{
    switch (m_type)
    {
    case Type_DecimalBallList:
        return "DecimalBallList";

    case Type_DecimalTriangleList:
        return "DecimalTriangleList";
    }

    return "<unknown>";
}

DecimalBallListPtr SceneObject::decimalBallList() const
{
    return m_decimalBallList;
}

DecimalTriangleListPtr SceneObject::decimalTriangleList() const
{
    return m_decimalTriangleList;
}

void SceneObject::setRotating(bool rotating)
{
    m_rotating = rotating;
}

bool SceneObject::isRotating() const
{
    return m_rotating;
}

void SceneObject::setVisible(bool visible)
{
    m_visible = visible;
}

bool SceneObject::isVisible() const
{
    return m_visible;
}

void SceneObject::setColor(QColor color)
{
    m_color = color;
}

QColor SceneObject::color() const
{
    return m_color;
}

std::pair<SceneObjectPtr, SceneObjectPtr> SceneObject::loadFromDirectory(const char *directory, QWidget *parent)
{
    Q_UNUSED(parent);

    std::string obstacleFileName = std::string(directory) + "/" + "obstacle.txt";
    std::string robotFileName = std::string(directory) + "/" + "robot.txt";

    SceneLoaderPtr obstacle = SceneLoader::load(obstacleFileName.c_str());
    SceneLoaderPtr robot = SceneLoader::load(robotFileName.c_str());

    if (!obstacle || !robot)
        return std::make_pair(SceneObjectPtr(), SceneObjectPtr());

    // convert to triangle lists
    DecimalTriangleListPtr robotTriangleList = sceneLoaderObjectToTriangleList(robot);
    DecimalTriangleListPtr obstacleTriangleList = sceneLoaderObjectToTriangleList(obstacle);

    // success
    SceneObjectPtr robotSceneObject(new SceneObject(robotTriangleList));
    robotSceneObject->setRotating(true);

    SceneObjectPtr obstacleSceneObject(new SceneObject(obstacleTriangleList));
    obstacleSceneObject->setRotating(false);

    return std::make_pair(robotSceneObject, obstacleSceneObject);
}

SceneObjectPtr SceneObject::loadFromSphereTree(const char *fileName, QWidget *parent)
{
    SphereTreeLoader loader;

    // renormalization
    bool normalize = QMessageBox::question(parent,
                                           QObject::tr("Normalize sphere tree"),
                                           QObject::tr("Do you want to normalize sphere tree to unit box?"),
                                           QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;

    if (!loader.loadFromFile(fileName, normalize))
    {
        QMessageBox::warning(parent,
                             QObject::tr("Open sphere tree"),
                             QObject::tr("Failed to load file"),
                             QMessageBox::Ok);

        return SceneObjectPtr();
    }

    // choose level
    bool ok = false;

    int level = QInputDialog::getInt(parent,
                                     QObject::tr("Select sphere tree level"),
                                     QObject::tr("Select level"),
                                     loader.numberOfLevels() - 1,
                                     0,
                                     loader.numberOfLevels() - 1,
                                     1,
                                     &ok);

    if (!ok)
        return SceneObjectPtr();

    // success
    return SceneObjectPtr(new SceneObject(DecimalBallListPtr(new DecimalBallList(loader.level(level)))));
}

SceneObjectPtr SceneObject::loadFromText(const char *fileName, QWidget *parent)
{
    Q_UNUSED(parent);

    SceneLoaderPtr mesh = SceneLoader::load(fileName);

    if (!mesh)
        return SceneObjectPtr();

    // convert to triangle lists
    DecimalTriangleListPtr meshTriangleList = sceneLoaderObjectToTriangleList(mesh);
    return SceneObjectPtr(new SceneObject(meshTriangleList));
}

void SceneObject::saveToStream(QDataStream &dataStream) const
{
    // save type
    dataStream << static_cast<int>(m_type);

    // save data
    switch (m_type)
    {
    case Type_DecimalBallList:
        dataStream << static_cast<int>(m_decimalBallList->size());

        for (DecimalBallList::const_iterator it = m_decimalBallList->begin();
             it != m_decimalBallList->end(); ++it)
        {
            dataStream << it->center().x();
            dataStream << it->center().y();
            dataStream << it->center().z();
            dataStream << it->radius();
        }

        break;

    case Type_DecimalTriangleList:
        dataStream << static_cast<int>(m_decimalTriangleList->size());

        for (DecimalTriangleList::const_iterator it = m_decimalTriangleList->begin();
             it != m_decimalTriangleList->end(); ++it)
        {
            dataStream << it->vertex(0).x();
            dataStream << it->vertex(0).y();
            dataStream << it->vertex(0).z();
            dataStream << it->vertex(1).x();
            dataStream << it->vertex(1).y();
            dataStream << it->vertex(1).z();
            dataStream << it->vertex(2).x();
            dataStream << it->vertex(2).y();
            dataStream << it->vertex(2).z();
        }

        break;
    }

    // save other data
    dataStream << m_rotating;
    dataStream << m_visible;
    dataStream << m_color;
}

SceneObjectPtr SceneObject::loadFromStream(QDataStream &dataStream)
{
    // load type
    int rawType;
    dataStream >> rawType;
    if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

    Type type = static_cast<Type>(rawType);

    // load data
    SceneObjectPtr sceneObject;

    switch (type)
    {
    case Type_DecimalBallList:
        {
            int count;
            dataStream >> count;
            if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

            DecimalBallListPtr ballList(new DecimalBallList());

            for (int i = 0; i < count; ++i)
            {
                QDecimal x, y, z, r;

                dataStream >> x;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> y;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> z;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> r;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                ballList->push_back(DecimalBall(DecimalVector(x, y, z), r));
            }

            sceneObject.reset(new SceneObject(ballList));
        }
        break;

    case Type_DecimalTriangleList:
        {
            int count;
            dataStream >> count;
            if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

            DecimalTriangleListPtr triangleList(new DecimalTriangleList());

            for (int i = 0; i < count; ++i)
            {
                QDecimal x0, y0, z0, x1, y1, z1, x2, y2, z2;

                dataStream >> x0;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> y0;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> z0;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> x1;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> y1;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> z1;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> x2;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> y2;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                dataStream >> z2;
                if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

                triangleList->push_back(DecimalTriangle(DecimalVector(x0, y0, z0),
                                                        DecimalVector(x1, y1, z1),
                                                        DecimalVector(x2, y2, z2)));
            }

            sceneObject.reset(new SceneObject(triangleList));
        }
        break;
    }

    // load other data
    bool rotating;
    dataStream >> rotating;
    if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

    bool visible;
    dataStream >> visible;
    if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

    QColor color;
    dataStream >> color;
    if (dataStream.status() != QDataStream::Ok) return SceneObjectPtr();

    // create result
    sceneObject->m_rotating = rotating;
    sceneObject->m_visible = visible;
    sceneObject->m_color = color;

    return sceneObject;
}

SceneObject::SceneObject(DecimalBallListPtr ballList)
    : m_type(Type_DecimalBallList),
      m_decimalBallList(ballList),
      m_rotating(false),
      m_visible(true)
{
}

SceneObject::SceneObject(DecimalTriangleListPtr triangleList)
    : m_type(Type_DecimalTriangleList),
      m_decimalTriangleList(triangleList),
      m_rotating(false),
      m_visible(true)
{
}
