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
#include "multisplitter.h"
#include <QGridLayout>

SyncSplitter::SyncSplitter(QWidget *parent)
    : QSplitter(parent),
      m_syncingSplitter(NULL)
{
}

SyncSplitter::SyncSplitter(Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent),
      m_syncingSplitter(NULL)
{
}

void SyncSplitter::setSyncSplitter(QSplitter *splitter)
{
    disconnectFromSyncSplitter();
    connectToSyncSplitter(splitter);
}

QSplitter *SyncSplitter::syncingSplitter() const
{
    return m_syncingSplitter;
}

void SyncSplitter::connectToSyncSplitter(QSplitter *splitter)
{
    Q_ASSERT(m_syncingSplitter == NULL);

    QObject::connect(splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(syncingSplitterMoved(int, int)));
    m_syncingSplitter = splitter;
}

void SyncSplitter::disconnectFromSyncSplitter()
{
    if (m_syncingSplitter == NULL)
        return;

    QObject::disconnect(m_syncingSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(syncingSplitterMoved(int, int)));
    m_syncingSplitter = NULL;
}

void SyncSplitter::syncingSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);

    // Affect only when changed
    QList<int> otherSizes = m_syncingSplitter->sizes();

    if (sizes() == otherSizes)
        return;

    // Pass synchronizing splitter moves to this splitter
    setSizes(otherSizes);
}

QuadSplitter::QuadSplitter(QWidget *topLeftWidget,
                             QWidget *topRightWidget,
                             QWidget *bottomLeftWidget,
                             QWidget *bottomRightWidget,
                             QWidget *parent)
    : QWidget(parent),
      m_topLeftWidget(topLeftWidget),
      m_bottomLeftWidget(topRightWidget),
      m_topRightWidget(bottomLeftWidget),
      m_bottomRightWidget(bottomRightWidget),
      m_maximized(false)
{
    m_splitterMain = new SyncSplitter(Qt::Horizontal, this);
    m_splitterLeft = new SyncSplitter(Qt::Vertical, this);
    m_splitterRight = new SyncSplitter(Qt::Vertical, this);

    m_splitterMain->addWidget(m_splitterLeft);
    m_splitterMain->addWidget(m_splitterRight);

    m_splitterLeft->addWidget(topLeftWidget);
    m_splitterLeft->addWidget(bottomLeftWidget);
    m_splitterRight->addWidget(topRightWidget);
    m_splitterRight->addWidget(bottomRightWidget);

    m_splitterLeft->setSyncSplitter(m_splitterRight);
    m_splitterRight->setSyncSplitter(m_splitterLeft);

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_splitterMain);
    setLayout(layout);
}

QuadSplitter::~QuadSplitter()
{
}

void QuadSplitter::toggleRequested(QWidget *widget)
{
    Q_ASSERT(widget == m_topLeftWidget || widget == m_bottomLeftWidget ||
             widget == m_topRightWidget || widget == m_bottomRightWidget);

    if (m_maximized)
    {
        // Return to default confguration
        m_topLeftWidget->show();
        m_bottomLeftWidget->show();
        m_topRightWidget->show();
        m_bottomRightWidget->show();
    }
    else
    {
        // Maximize selected widget
        m_topLeftWidget->hide();
        m_bottomLeftWidget->hide();
        m_topRightWidget->hide();
        m_bottomRightWidget->hide();

        widget->show();
    }

    // Toggle maximized property
    m_maximized = !m_maximized;
}

void DoubleSplitter::toggleRequested(QWidget *widget)
{
    Q_ASSERT(widget == m_leftWidget || widget == m_rightWidget);

    if (m_maximized)
    {
        // Return to default confguration
        m_leftWidget->show();
        m_rightWidget->show();
    }
    else
    {
        // Maximize selected widget
        m_leftWidget->hide();
        m_rightWidget->hide();

        widget->show();
    }

    // Toggle maximized property
    m_maximized = !m_maximized;
}

DoubleSplitter::DoubleSplitter(QWidget *leftWidget, QWidget *rightWidget, QWidget *parent)
    : QWidget(parent),
      m_leftWidget(leftWidget),
      m_rightWidget(rightWidget),
      m_maximized(false)
{
    m_splitter = new SyncSplitter(Qt::Horizontal, this);

    m_splitter->addWidget(leftWidget);
    m_splitter->addWidget(rightWidget);

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_splitter);
    setLayout(layout);
}

void DoubleSplitter::toggleFullscreenLeft()
{
    m_rightWidget->setVisible(!m_rightWidget->isVisible());
}

void DoubleSplitter::toggleFullscreenRight()
{
    m_leftWidget->setVisible(!m_leftWidget->isVisible());
}
