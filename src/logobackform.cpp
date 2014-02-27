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
#include "logobackform.h"
#include <QPainter>
#include <QLinearGradient>

LogoBackForm::LogoBackForm(QWidget *parent) :
    QWidget(parent)
{
    createShade();
}

LogoBackForm::~LogoBackForm()
{
}

void LogoBackForm::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawImage(0, 0, m_shade);
}

void LogoBackForm::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    createShade();
}

void LogoBackForm::createShade()
{
    QGradientStops gradientStops;

    gradientStops << QGradientStop(0.00, QColor::fromRgba(0));
    gradientStops << QGradientStop(1.00, QColor::fromRgba(0xffb9d9e6));

    QLinearGradient alphaGradient = QLinearGradient(0, 0, width(), 0);

    for (int i=0; i<gradientStops.size(); ++i)
    {
        QColor color = gradientStops.at(i).second;
        alphaGradient.setColorAt(gradientStops.at(i).first, QColor(color.red(), color.green(), color.blue()));
    }

    m_shade = QImage(size(), QImage::Format_ARGB32_Premultiplied);
    m_shade.fill(0);

    QPainter painter(&m_shade);
    painter.fillRect(rect(), alphaGradient);

    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);

    QLinearGradient fadeGradient(0, 0, 0, height());
    fadeGradient.setColorAt(0.9, QColor(0, 0, 0, 255));
    fadeGradient.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(rect(), fadeGradient);
}
