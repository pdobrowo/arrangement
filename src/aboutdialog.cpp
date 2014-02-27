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
#include "aboutdialog.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include "ui_aboutdialog.h"

namespace // anonymous
{
const char *BUILD_TIMESTAMP = __TIMESTAMP__;
} // namespace anonymous

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // app version
    ui->labelAppVer->setText(BUILD_TIMESTAMP);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_labelWeb_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}
