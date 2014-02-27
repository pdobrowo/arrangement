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
#ifndef QLOG4CXX_H
#define QLOG4CXX_H

#include <QObject>
#include <QString>

class QLog4cxx : public QObject
{
    Q_OBJECT
public:
    explicit QLog4cxx(QObject *parent = 0);
    ~QLog4cxx();
    
    static void configure();
    static void deconfigure();

    static QLog4cxx *instance();

private:
    void internalLogMessage(QString loggerName, QString level, long long deltaTimeMicroSec, QString message);

signals:
    void logMessage(QString loggerName, QString level, long long deltaTimeMicroSec, QString message);
};

#endif // QLOG4CXX_H
