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
#ifndef MATERIAL_H
#define MATERIAL_H

#include <QColor>

namespace Material
{
// shininess
const qreal SHININESS_ZERO  = 0.0f;
const qreal SHININESS_LOW   = 5.0f;
const qreal SHININESS_HIGH  = 100.0f;

void setDiffuse(const QColor &diffuse);

void setDiffuseSpecularShininess(const QColor &diffuse,
                                 const QColor &specular = Qt::white,
                                 qreal shininess = SHININESS_HIGH);
} // namespace Material

#endif // MATERIAL_H
