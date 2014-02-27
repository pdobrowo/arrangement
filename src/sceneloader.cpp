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
#include "sceneloader.h"
#include "tritripredicatelist.h"
#include <fstream>

SceneLoader::SceneLoader(const DecimalVectorList &vertices, DecimalFaceList &faces)
    : m_vertices(vertices),
      m_faces(faces)
{
}

SceneLoaderPtr SceneLoader::load(const char *fileName)
{
    std::ifstream file(fileName);

    if (!file.is_open())
        SceneLoaderPtr();

    // parse file
    size_t num_vertices;

    if (!(file >> num_vertices))
        return SceneLoaderPtr();

    DecimalVectorList vertices;
    vertices.reserve(num_vertices);

    for (size_t i = 0; i < num_vertices; ++i)
    {
        std::string x, y, z;

        // read data as strings
        if (!(file >> x) || !(file >> y) || !(file >> z))
            return SceneLoaderPtr();

        // and parse to decimals
        QDecimal decimalX, decimalY, decimalZ;

        decimalX.fromString(x.c_str());
        decimalY.fromString(y.c_str());
        decimalZ.fromString(z.c_str());

        vertices.push_back(DecimalVector(decimalX, decimalY, decimalZ));
    }

    size_t num_faces;

    if (!(file >> num_faces))
        return SceneLoaderPtr();

    DecimalFaceList faces;
    faces.reserve(num_faces);

    for (size_t i = 0; i < num_faces; ++i)
    {
        size_t a, b, c;

        if (!(file >> a) || !(file >> b) || !(file >> c))
            return SceneLoaderPtr();

        faces.push_back(DecimalFace(a, b, c));
    }

    return SceneLoaderPtr(new SceneLoader(vertices, faces));
}

const DecimalVectorList & SceneLoader::vertices() const
{
    return m_vertices;
}

const DecimalFaceList & SceneLoader::faces() const
{
    return m_faces;
}
