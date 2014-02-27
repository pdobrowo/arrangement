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
#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "decimalscene.h"
#include <boost/shared_ptr.hpp>
#include <string>

class SceneLoader;
typedef boost::shared_ptr<SceneLoader> SceneLoaderPtr;

class SceneLoader
{
public:
    static SceneLoaderPtr       load(const char *fileName);

    const DecimalVectorList &   vertices() const;
    const DecimalFaceList &     faces() const;

private:
    DecimalVectorList           m_vertices;
    DecimalFaceList             m_faces;

    SceneLoader(const DecimalVectorList &vertices, DecimalFaceList &faces);
};

#endif // SCENELOADER_H
