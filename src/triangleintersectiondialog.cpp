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
#include "triangleintersectiondialog.h"
#include "vectorvalidator.h"
#include "tritripredicatelist.h"
#include <cs/Benchmark.h>
#include "ui_triangleintersectiondialog.h"

TriangleIntersectionDialog::TriangleIntersectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TriangleIntersectionDialog)
{
    ui->setupUi(this);

    // validators
    ui->lineEditA->setValidator(new VectorValidator());
    ui->lineEditB->setValidator(new VectorValidator());
    ui->lineEditC->setValidator(new VectorValidator());
    ui->lineEditK->setValidator(new VectorValidator());
    ui->lineEditL->setValidator(new VectorValidator());
    ui->lineEditM->setValidator(new VectorValidator());
}

TriangleIntersectionDialog::~TriangleIntersectionDialog()
{
    delete ui;
}

std::vector<VariantPredicate> TriangleIntersectionDialog::predicateList() const
{
    Vector_3_Z a = toVector_3(ui->lineEditA->text().toStdString());
    Vector_3_Z b = toVector_3(ui->lineEditB->text().toStdString());
    Vector_3_Z c = toVector_3(ui->lineEditC->text().toStdString());
    Vector_3_Z k = toVector_3(ui->lineEditK->text().toStdString());
    Vector_3_Z l = toVector_3(ui->lineEditL->text().toStdString());
    Vector_3_Z m = toVector_3(ui->lineEditM->text().toStdString());

    return triTriPredicateList(a, b, c, k, l, m);
}

std::pair<Mesh_smooth_triangle_3_Z, Mesh_smooth_triangle_3_Z> TriangleIntersectionDialog::trianglePair() const
{
    Mesh_point_3_Z a = toMesh_point_3(ui->lineEditA->text().toStdString());
    Mesh_point_3_Z b = toMesh_point_3(ui->lineEditB->text().toStdString());
    Mesh_point_3_Z c = toMesh_point_3(ui->lineEditC->text().toStdString());
    Mesh_point_3_Z k = toMesh_point_3(ui->lineEditK->text().toStdString());
    Mesh_point_3_Z l = toMesh_point_3(ui->lineEditL->text().toStdString());
    Mesh_point_3_Z m = toMesh_point_3(ui->lineEditM->text().toStdString());

    return std::make_pair(Mesh_smooth_triangle_3_Z(a, b, c),
                          Mesh_smooth_triangle_3_Z(k, l, m));
}

void TriangleIntersectionDialog::on_pushButtonExample1_clicked()
{
    ui->lineEditA->setText("1; 0; 0");
    ui->lineEditB->setText("0; 1; 0");
    ui->lineEditC->setText("0; 0; 1");
    ui->lineEditK->setText("1; 1; 0");
    ui->lineEditL->setText("0; 1; 1");
    ui->lineEditM->setText("1; 0; 1");
}

void TriangleIntersectionDialog::on_pushButtonRandom_clicked()
{
    Vector_3_Z k = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z l = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z m = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z a = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z b = CS::Benchmark<Kernel_Z>::randomVector();
    Vector_3_Z c = CS::Benchmark<Kernel_Z>::randomVector();

    long kx = 0, ky = 0, kz = 0;
    long lx = 0, ly = 0, lz = 0;
    long mx = 0, my = 0, mz = 0;
    long ax = 0, ay = 0, az = 0;
    long bx = 0, by = 0, bz = 0;
    long cx = 0, cy = 0, cz = 0;

    k.x().longify(kx); k.y().longify(ky); k.z().longify(kz);
    l.x().longify(lx); l.y().longify(ly); l.z().longify(lz);
    m.x().longify(mx); m.y().longify(my); m.z().longify(mz);
    a.x().longify(ax); a.y().longify(ay); a.z().longify(az);
    b.x().longify(bx); b.y().longify(by); b.z().longify(bz);
    c.x().longify(cx); c.y().longify(cy); c.z().longify(cz);

    ui->lineEditK->setText(QString("%1; %2; %3").arg(kx).arg(ky).arg(kz));
    ui->lineEditL->setText(QString("%1; %2; %3").arg(lx).arg(ly).arg(lz));
    ui->lineEditM->setText(QString("%1; %2; %3").arg(mx).arg(my).arg(mz));
    ui->lineEditA->setText(QString("%1; %2; %3").arg(ax).arg(ay).arg(az));
    ui->lineEditB->setText(QString("%1; %2; %3").arg(bx).arg(by).arg(bz));
    ui->lineEditC->setText(QString("%1; %2; %3").arg(cx).arg(cy).arg(cz));
}
