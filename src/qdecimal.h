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
#ifndef QDECIMAL_H
#define QDECIMAL_H

#include <qdecimal/QDecNumber.hh>
#include <QDataStream>
#include <QByteArray>

typedef QDecNumber QDecimal;

inline QDataStream &operator <<(QDataStream &dataStream, const QDecimal &decimal)
{
    QByteArray byteArray = decimal.toString();
    QString rep = QString::fromLatin1(byteArray.data(), byteArray.size());
    dataStream << rep;
    return dataStream;
}

inline QDataStream &operator >>(QDataStream &dataStream, QDecimal &decimal)
{
    QString rep;
    dataStream >> rep;
    decimal.fromString(rep.toLatin1().data());
    return dataStream;
}

#endif // QDECIMAL_H
