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
#ifndef CONFIGURATIONOBJECTDIALOG_H
#define CONFIGURATIONOBJECTDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigurationObjectDialog;
}

class ConfigurationObjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    ConfigurationObjectDialog(
            QString type,
            bool isObjectVisible,
            QString information,
            QWidget *parent = 0);

    ~ConfigurationObjectDialog();

    bool isObjectVisible() const;

private:
    Ui::ConfigurationObjectDialog *ui;
};

#endif // CONFIGURATIONOBJECTDIALOG_H
