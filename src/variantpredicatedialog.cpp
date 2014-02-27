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
#include "variantpredicatedialog.h"
#include "ui_variantpredicatedialog.h"

VariantPredicateDialog::VariantPredicateDialog(VariantPredicate::Type type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VariantPredicateDialog)
{
    ui->setupUi(this);

    // set default type
    switch (type)
    {
    case VariantPredicate::None:
        assert(0);
        break;

    case VariantPredicate::Halfspace:
        ui->tabWidget->setCurrentIndex(0);
        break;

    case VariantPredicate::Screw:
        ui->tabWidget->setCurrentIndex(1);
        break;

    case VariantPredicate::Generic:
        ui->tabWidget->setCurrentIndex(2);
        break;
    }
}

VariantPredicateDialog::~VariantPredicateDialog()
{
    delete ui;
}

VariantPredicate VariantPredicateDialog::predicate() const
{
    switch (ui->tabWidget->currentIndex())
    {
        case 0: return ui->widgetPredicateH->predicate();
        case 1: return ui->widgetPredicateS->predicate();
        case 2: return ui->widgetPredicateG->predicate();
        default: return VariantPredicate();
    }
}
