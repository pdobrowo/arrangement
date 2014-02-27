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
#include <QApplication>
#include <QtGlobal>
#include <QIcon>
#include <QGLFormat>
#include "mainwindow.h"
#include "qlog4cxx.h"
#include <unistd.h>
#include <clocale>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    // setup locale
    setlocale(LC_ALL, "C");

    // configure logger
    QLog4cxx::configure();

    // set multisampling as default
    if (0)
    {
        QGLFormat glFormat = QGLFormat::defaultFormat();
        glFormat.setSampleBuffers(true);
        //glFormat.setSamples(4);
        QGLFormat::setDefaultFormat(glFormat);
    }

    // set application icon
    QApplication::setWindowIcon(QIcon(":/resource/img/recalculate.png"));

    // randomize
    qsrand(static_cast<uint>(getpid()));

    // start application
    int result;
    {
        MainWindow mainWindow;
        mainWindow.show();

        result = application.exec();
    }

    // deconfigure logger
    QLog4cxx::deconfigure();

    // done
    return result;
}
