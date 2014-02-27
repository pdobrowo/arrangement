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
#ifndef MULTISPLITTER_H
#define MULTISPLITTER_H

#include <QWidget>
#include <QSplitter>

class SyncSplitter
    : public QSplitter
{
    Q_OBJECT

private:
    QSplitter *m_syncingSplitter;

    void connectToSyncSplitter(QSplitter *splitter);
    void disconnectFromSyncSplitter();

private slots:
    void syncingSplitterMoved(int pos, int index);

public:
    SyncSplitter(QWidget *parent);
    SyncSplitter(Qt::Orientation orientation, QWidget *parent);

    void setSyncSplitter(QSplitter *splitter);
    QSplitter *syncingSplitter() const;
};

class DoubleSplitter
    : public QWidget
{
    Q_OBJECT

private:
    // Cached widgets
    QWidget *m_leftWidget;
    QWidget *m_rightWidget;

    QSplitter *m_splitter;

    // Maximize toogle
    bool m_maximized;

public:
    DoubleSplitter(QWidget *leftWidget,
                   QWidget *rightWidget,
                   QWidget *parent);

    void toggleFullscreenLeft();
    void toggleFullscreenRight();

public slots:
    void toggleRequested(QWidget *widget);
};

class QuadSplitter
    : public QWidget
{
    Q_OBJECT

private:
    // Cached widgets
    QWidget *m_topLeftWidget;
    QWidget *m_bottomLeftWidget;
    QWidget *m_topRightWidget;
    QWidget *m_bottomRightWidget;

    // Internal splitters
    SyncSplitter *m_splitterMain;
    SyncSplitter *m_splitterLeft;
    SyncSplitter *m_splitterRight;

    // Maximize toogle
    bool m_maximized;

public:
    QuadSplitter(QWidget *topLeftWidget,
                 QWidget *topRightWidget,
                 QWidget *bottomLeftWidget,
                 QWidget *bottomRightWidget,
                 QWidget *parent);

    virtual ~QuadSplitter();

public slots:
    void toggleRequested(QWidget *widget);
};

#endif // MULTISPLITTER_H
