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
#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "kernel.h"
#include "decimalscene.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <utility>
#include <QString>
#include <QColor>

class QWidget;

class SceneObject;
typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

class SceneObject
    : private boost::noncopyable
{
public:
    enum Type
    {
        Type_DecimalBallList,
        Type_DecimalTriangleList
    };

    Type                    type() const;
    QString                 typeString() const;

    DecimalBallListPtr      decimalBallList() const;
    DecimalTriangleListPtr  decimalTriangleList() const;

    void                    setRotating(bool rotating);
    bool                    isRotating() const;

    void                    setVisible(bool visible);
    bool                    isVisible() const;

    void                    setColor(QColor color);
    QColor                  color() const;

    static std::pair<SceneObjectPtr, SceneObjectPtr>    loadFromDirectory(const char *directory, QWidget *parent);
    static SceneObjectPtr                               loadFromSphereTree(const char *fileName, QWidget *parent);
    static SceneObjectPtr                               loadFromText(const char *fileName, QWidget *parent);

    // arr format support
    void                                                saveToStream(QDataStream &dataStream) const;
    static SceneObjectPtr                               loadFromStream(QDataStream &dataStream);

private:
    explicit SceneObject(DecimalBallListPtr decimalBallList);
    explicit SceneObject(DecimalTriangleListPtr decimalTriangleList);

    Type                    m_type;
    DecimalBallListPtr      m_decimalBallList;
    DecimalTriangleListPtr  m_decimalTriangleList;

    bool                    m_rotating;
    bool                    m_visible;

    QColor                  m_color;
};

#endif // SCENEOBJECT_H
