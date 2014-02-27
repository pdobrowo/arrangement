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
#include "material.h"
#include <GL/gl.h>

namespace Material
{
namespace // anonymous
{
static float NO_MATERIAL[] = { 0.0f, 0.0f, 0.0f, 1.0f };

static void setMaterial(GLfloat ambient[4],
                        GLfloat diffuse[4],
                        GLfloat specular[4],
                        GLfloat shininess,
                        GLfloat emission[4])
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
}
} // namespace anonymous

void setDiffuse(const QColor &diffuse)
{
    GLfloat diffuseMaterial[] = { diffuse.redF(),
                                  diffuse.greenF(),
                                  diffuse.blueF(),
                                  1.0f};

    setMaterial(NO_MATERIAL,
                diffuseMaterial,
                NO_MATERIAL,
                SHININESS_ZERO,
                NO_MATERIAL);
}

void setDiffuseSpecularShininess(
    const QColor &diffuse,
    const QColor &specular,
    qreal shininess)
{
    GLfloat diffuseMaterial[] = { diffuse.redF(),
                                  diffuse.greenF(),
                                  diffuse.blueF(),
                                  1.0f};

    GLfloat specularMaterial[] = { specular.redF(),
                                   specular.greenF(),
                                   specular.blueF(),
                                   1.0f};

    setMaterial(NO_MATERIAL,
                diffuseMaterial,
                specularMaterial,
                shininess,
                NO_MATERIAL);
}
} // namespace Material
