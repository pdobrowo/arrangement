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
#include "spheretreeloader.h"
#include <sstream>
#include <fstream>

SphereTreeLoader::SphereTreeLoader()
    : m_numberOfLevels(0),
      m_levelDegree(0)
{
}

bool SphereTreeLoader::loadFromFile(const char *fileName, bool normalize)
{
    std::ifstream file(fileName);

    if (!file.is_open())
        return false;

    std::string line;

    if (!std::getline(file, line))
        return false;

    int numberOfLevels, levelDegree;

    std::istringstream header(line);

    header >> numberOfLevels >> levelDegree;

    if (!header)
        return false;

    if (numberOfLevels <= 0 || levelDegree <= 0)
        return false;

    m_numberOfLevels = static_cast<size_t>(numberOfLevels);
    m_levelDegree = static_cast<size_t>(levelDegree);

    int numberOfNodes = 1;
    QDecimal maxAbsoluteCoordinate = 0;

    for (int k = 0; k < numberOfLevels; k++)
    {
        Level level;

        for (int i = 0; i < numberOfNodes; i++)
        {
            if (!std::getline(file, line))
                return false;

            //  read the sphere
            std::string x, y, z, r, dummy;
            std::istringstream parser(line);

            parser >> x >> y >> z >> r >> dummy;

            if (parser)
            {
                QDecimal decimalX(x.c_str());
                QDecimal decimalY(y.c_str());
                QDecimal decimalZ(z.c_str());
                QDecimal decimalR(r.c_str());

                level.push_back(DecimalBall(DecimalVector(decimalX, decimalY, decimalZ), decimalR));

                QDecimal absoluteDecimalX(decimalX.abs());
                QDecimal absoluteDecimalY(decimalY.abs());
                QDecimal absoluteDecimalZ(decimalZ.abs());

                if (absoluteDecimalX > maxAbsoluteCoordinate) maxAbsoluteCoordinate = absoluteDecimalX;
                if (absoluteDecimalY > maxAbsoluteCoordinate) maxAbsoluteCoordinate = absoluteDecimalY;
                if (absoluteDecimalZ > maxAbsoluteCoordinate) maxAbsoluteCoordinate = absoluteDecimalZ;
            }
        }

        // accumulate level
        m_levels.push_back(level);

        // next degree
        numberOfNodes *= levelDegree;
    }

    // normalize if needed
    if (normalize)
    {
        QDecimal scale = QDecimal(1) / maxAbsoluteCoordinate;

        for (typename Levels::iterator level_iterator = m_levels.begin(); level_iterator != m_levels.end(); ++level_iterator)
            for (typename Level::iterator ball_iterator = level_iterator->begin(); ball_iterator != level_iterator->end(); ++ball_iterator)
                ball_iterator->scale(scale);
    }

    return true;
}

size_t SphereTreeLoader::numberOfLevels() const
{
    return m_numberOfLevels;
}

size_t SphereTreeLoader::levelDegree() const
{
    return m_levelDegree;
}

typename SphereTreeLoader::const_iterator SphereTreeLoader::level_begin(size_t level) const
{
    return m_levels[level].begin();
}

typename SphereTreeLoader::const_iterator SphereTreeLoader::level_end(size_t level) const
{
    return m_levels[level].end();
}

DecimalBallList SphereTreeLoader::level(size_t level) const
{
    return m_levels[level];
}
