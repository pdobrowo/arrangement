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
#ifndef BENCHMARKDIALOG_H
#define BENCHMARKDIALOG_H

#include "variantpredicate.h"
#include <QRandomGenerator>
#include <QScopedPointer>
#include <QDialog>
#include <QThread>
#include <string>

namespace Ui {
    class BenchmarkDialog;
}

class TestThread: public QThread
{
    Q_OBJECT

private:
    int m_test;

    void prereport(const std::string &message);

public:
    TestThread(int test);
    ~TestThread();

    virtual void run();

signals:
    void report(QString message);
    void success();
};

class BenchmarkDialog : public QDialog
{
    Q_OBJECT

private slots:
    void report(QString message);
    void success();

public:
    explicit BenchmarkDialog(QWidget *parent = 0);
    ~BenchmarkDialog();

private:
    QScopedPointer<TestThread> m_test;
    QRandomGenerator m_rand;

private slots:
    void on_pushButtonRun_clicked();
    void on_pushButtonAbort_clicked();

private:
    Ui::BenchmarkDialog *ui;
};

#endif // BENCHMARKDIALOG_H
