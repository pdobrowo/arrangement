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
#include "sceneobjectdialog.h"
#include "ui_sceneobjectdialog.h"

SceneObjectDialog::SceneObjectDialog(QString type, bool visible, bool rotating, QColor color, QString information, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SceneObjectDialog)
{
    ui->setupUi(this);

    ui->labelTypeValue->setText(type);
    ui->checkBoxVisible->setChecked(visible);
    ui->checkBoxRotating->setChecked(rotating);
    ui->widgetColor->setColor(color);
    ui->plainTextEditGeometry->setPlainText(information);
}

SceneObjectDialog::~SceneObjectDialog()
{
    delete ui;
}

bool SceneObjectDialog::isObjectVisible() const
{
    return ui->checkBoxVisible->isChecked();
}

bool SceneObjectDialog::isObjectRotating() const
{
    return ui->checkBoxRotating->isChecked();
}

QColor SceneObjectDialog::objectColor() const
{
    return ui->widgetColor->color();
}
