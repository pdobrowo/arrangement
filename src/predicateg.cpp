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
#include "predicateg.h"
#include "numbervalidator.h"
#include "vectorvalidator.h"
#include "ui_predicateg.h"
#include <cs/Benchmark.h>

PredicateG::PredicateG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PredicateG)
{
    ui->setupUi(this);

    // validators
    ui->lineEditPointA->setValidator(new VectorValidator());
    ui->lineEditPointB->setValidator(new VectorValidator());
    ui->lineEditPointK->setValidator(new VectorValidator());
    ui->lineEditPointL->setValidator(new VectorValidator());
    ui->lineEditDistance->setValidator(new NumberValidator());
}

PredicateG::~PredicateG()
{
    delete ui;
}

void PredicateG::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

VariantPredicate PredicateG::predicate() const
{
    VariantPredicate predicate;

    predicate.type = VariantPredicate::Generic;

    predicate.g3 = Predicate_G_3_Z(toVector_3(ui->lineEditPointK->text().toStdString()),
                                 toVector_3(ui->lineEditPointL->text().toStdString()),
                                 toVector_3(ui->lineEditPointA->text().toStdString()),
                                 toVector_3(ui->lineEditPointB->text().toStdString()),
                                 toNumber(ui->lineEditDistance->text().toStdString()));

    return predicate;
}

void PredicateG::on_pushButtonExample1_clicked()
{
    ui->lineEditPointK->setText(tr("0; 2; 0"));
    ui->lineEditPointL->setText(tr("0; 0; 2"));
    ui->lineEditPointA->setText(tr("2; 2; 0"));
    ui->lineEditPointB->setText(tr("2; 0; 2"));
    ui->lineEditDistance->setText(tr("1"));
}

void PredicateG::on_pushButtonRandom_clicked()
{
    Vector_3_Z k = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z l = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z a = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z b = CS::Benchmark<Kernel_Z>::randomVector();
    bigint c = CS::Benchmark<Kernel_Z>::randomPoint();

    long kx = 0, ky = 0, kz = 0;
    long lx = 0, ly = 0, lz = 0;
    long ax = 0, ay = 0, az = 0;
    long bx = 0, by = 0, bz = 0;
    long cc = 0;

    k.x().longify(kx); k.y().longify(ky); k.z().longify(kz);
    l.x().longify(lx); l.y().longify(ly); l.z().longify(lz);
    a.x().longify(ax); a.y().longify(ay); a.z().longify(az);
    b.x().longify(bx); b.y().longify(by); b.z().longify(bz);
    c.longify(cc);

    ui->lineEditPointK->setText(QString("%1; %2; %3").arg(kx).arg(ky).arg(kz));
    ui->lineEditPointL->setText(QString("%1; %2; %3").arg(lx).arg(ly).arg(lz));
    ui->lineEditPointA->setText(QString("%1; %2; %3").arg(ax).arg(ay).arg(az));
    ui->lineEditPointB->setText(QString("%1; %2; %3").arg(bx).arg(by).arg(bz));
    ui->lineEditDistance->setText(QString("%1").arg(cc));
}
