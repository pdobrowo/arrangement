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
#ifndef SPHERETREELOADER_H
#define SPHERETREELOADER_H

#include "decimalscene.h"
#include <vector>
#include <cstddef>

class SphereTreeLoader
{
    typedef DecimalBallList                 Level;
    typedef std::vector<Level>              Levels;

public:
    typedef typename Level::const_iterator  const_iterator;

    SphereTreeLoader();

    bool            loadFromFile(const char *fileName, bool normalize = true);

    size_t          numberOfLevels() const;
    size_t          levelDegree() const;

    const_iterator  level_begin(size_t level) const;
    const_iterator  level_end(size_t level) const;

    DecimalBallList level(size_t level) const;

private:
    size_t          m_numberOfLevels;
    size_t          m_levelDegree;
    Levels          m_levels;
};

#endif // SPHERETREELOADER_H
