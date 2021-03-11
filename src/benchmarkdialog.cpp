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
#include "benchmarkdialog.h"
#include "sceneloader.h"
#include <cs/Benchmark.h>
#include <QtGlobal>
#include "ui_benchmarkdialog.h"

BenchmarkDialog::BenchmarkDialog(QWidget *parent) :
    QDialog(parent),
    m_rand(static_cast<quint32>(time(0))),
    ui(new Ui::BenchmarkDialog)
{
    ui->setupUi(this);

    // buttons
    ui->pushButtonRun->setEnabled(true);
    ui->pushButtonAbort->setEnabled(false);
}

BenchmarkDialog::~BenchmarkDialog()
{
    delete ui;
}

void BenchmarkDialog::on_pushButtonRun_clicked()
{
    ui->pushButtonRun->setEnabled(false);
    ui->pushButtonAbort->setEnabled(true);

    // prepare
    ui->plainTextEditOutput->clear();
    ui->labelStatus->setText(tr("Running..."));

    // run
    m_test.reset(new TestThread(ui->comboBoxScenario->currentIndex()));
    connect(m_test.data(), SIGNAL(report(QString)), this, SLOT(report(QString)));
    connect(m_test.data(), SIGNAL(success()), this, SLOT(success()));
    m_test->start();
}

void BenchmarkDialog::report(QString message)
{
    ui->plainTextEditOutput->appendPlainText(message);
}

TestThread::TestThread(int test)
    : m_test(test)
{
}

TestThread::~TestThread()
{
    terminate();
}

void TestThread::run()
{
    // take a test
    CS::Benchmark<Kernel_Z> benchmark;

    switch (m_test)
    {
        case 0: // H3 test
            using std::placeholders::_1;
            benchmark.random_H3_intersection_test(5, std::bind(&TestThread::prereport, this, _1));
            break;
    }

    emit success();
}

void TestThread::prereport(const std::string &message)
{
    emit report(QString::fromStdString(message));
}

void BenchmarkDialog::on_pushButtonAbort_clicked()
{
    m_test.reset();
    ui->pushButtonRun->setEnabled(true);
    ui->pushButtonAbort->setEnabled(false);

    ui->labelStatus->setText(QString());
}

void BenchmarkDialog::success()
{
    m_test.reset();
    ui->pushButtonRun->setEnabled(true);
    ui->pushButtonAbort->setEnabled(false);

    ui->labelStatus->setText(QString());
}
