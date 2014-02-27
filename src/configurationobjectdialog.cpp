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
#include "configurationobjectdialog.h"
#include "ui_configurationobjectdialog.h"

ConfigurationObjectDialog::ConfigurationObjectDialog(
        QString type,
        bool visible,
        QString information,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationObjectDialog)
{
    ui->setupUi(this);

    ui->labelTypeValue->setText(type);
    ui->checkBoxVisible->setChecked(visible);
    ui->plainTextEditGeometry->setPlainText(information);
}

ConfigurationObjectDialog::~ConfigurationObjectDialog()
{
    delete ui;
}

bool ConfigurationObjectDialog::isObjectVisible() const
{
    return ui->checkBoxVisible->isChecked();
}
