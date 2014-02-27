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
#include "predicateh.h"
#include "vectorvalidator.h"
#include "planevalidator.h"
#include "ui_predicateh.h"
#include <cs/Benchmark.h>

PredicateH::PredicateH(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PredicateH)
{
    ui->setupUi(this);

    // validators
    ui->lineEditBaseVector->setValidator(new VectorValidator());
    ui->lineEditPlane->setValidator(new PlaneValidator());
}

PredicateH::~PredicateH()
{
    delete ui;
}

void PredicateH::changeEvent(QEvent *e)
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

VariantPredicate PredicateH::predicate() const
{
    VariantPredicate predicate;

    predicate.type = VariantPredicate::Halfspace;

    predicate.h3 = Predicate_H_3_Z(toVector_3(ui->lineEditBaseVector->text().toStdString()),
                                 toPlane_3(ui->lineEditPlane->text().toStdString()));

    return predicate;
}

void PredicateH::on_pushButtonExample1_clicked()
{
    ui->lineEditBaseVector->setText(tr("1; 0; 0"));
    ui->lineEditPlane->setText(tr("0; 1; 0; 0"));
}

void PredicateH::on_pushButtonExample2_clicked()
{
    ui->lineEditBaseVector->setText(tr("0; 1; 0"));
    ui->lineEditPlane->setText(tr("0; 0; 1; 0"));
}

void PredicateH::on_pushButtonExample3_clicked()
{
    ui->lineEditBaseVector->setText(tr("0; 0; 1"));
    ui->lineEditPlane->setText(tr("1; 0; 0; 0"));
}

void PredicateH::on_pushButtonRandom_clicked()
{
    Vector_3_Z v = CS::Benchmark<Kernel_Z>::randomVector();
    Plane_3_Z p = CS::Benchmark<Kernel_Z>::randomPlane();

    long vx = 0, vy = 0, vz = 0;
    long pa = 0, pb = 0, pc = 0, pd = 0;

    v.x().longify(vx); v.y().longify(vy); v.z().longify(vz);
    p.a().longify(pa); p.b().longify(pb); p.c().longify(pc); p.d().longify(pd);

    ui->lineEditBaseVector->setText(QString("%1; %2; %3").arg(vx).arg(vy).arg(vz));
    ui->lineEditPlane->setText(tr("%1; %2; %3; %4").arg(pa).arg(pb).arg(pc).arg(pd));
}
