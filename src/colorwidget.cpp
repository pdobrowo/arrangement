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
#include "colorwidget.h"
#include <QColorDialog>
#include <QPainter>

ColorWidget::ColorWidget(QWidget *parent) :
    QWidget(parent),
    m_color(Qt::black)
{
}

void ColorWidget::setColor(QColor color)
{
    m_color = color;

    emit update();
}

QColor ColorWidget::color() const
{
    return m_color;
}

void ColorWidget::mousePressEvent(QMouseEvent *)
{
    QColorDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    setColor(dialog.selectedColor());
}

void ColorWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), m_color);
}
