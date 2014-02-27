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
#include "mainwindow.h"
#include "aboutdialog.h"
#include "variantpredicatedialog.h"
#include "benchmarkdialog.h"
#include "triangleintersectiondialog.h"
#include "clientform.h"
#include <cs/Loader_sphere_tree.h>
#include "ui_mainwindow.h"
#include "renderview.h"
#include "renderviewarcballcamera.h"
#include "renderviewflycamera.h"
#include "renderviewautocamera.h"
#include "sceneloader.h"
#include "multisplitter.h"
#include "tritripredicatelist.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <QInputDialog>
#include <QFrame>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QDoubleValidator>
#include <sstream>
#include <fstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup render view
    ClientForm *clientForm = new ClientForm();
    connect(clientForm, SIGNAL(toggleFullScreen()), this, SLOT(toggleFullScreen()));
    ui->mdiArea->addSubWindow(clientForm)->setWindowState(Qt::WindowMaximized);

    // setup criteria validators
//    ui->lineEditSurfaceMeshAngularBound->setValidator(new QDoubleValidator(1.0, 90.0, 8, ui->lineEditSurfaceMeshAngularBound));
//    ui->lineEditSurfaceMeshRadiusBound->setValidator(new QDoubleValidator(0.001, 0.1, 8, ui->lineEditSurfaceMeshRadiusBound));
//    ui->lineEditSurfaceMeshDistanceBound->setValidator(new QDoubleValidator(0.01, 0.1, 8, ui->lineEditSurfaceMeshDistanceBound));
//    ui->lineEditCurveMeshRadiusBound->setValidator(new QDoubleValidator(0.01, 0.1, 8, ui->lineEditCurveMeshRadiusBound));

    setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*
void MainWindow::showAddPredicateDialog(VariantPredicate::Type type)
{
    VariantPredicateDialog dialog(type, this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // add selected predicate
    addPredicateToScene(dialog.predicate());
}

void MainWindow::addPredicateToScene(VariantPredicate variantPredicate)
{
    // initialize stored predicate
    StoredPredicatePtr storedPredicate(new StoredPredicate());

    // get selected predicate
    storedPredicate->variantPredicate = variantPredicate;

    // read current mesh criteria
    storedPredicate->angularBound = ui->lineEditSurfaceMeshAngularBound->text().toDouble();
    storedPredicate->radiusBound = ui->lineEditSurfaceMeshRadiusBound->text().toDouble();
    storedPredicate->distanceBound = ui->lineEditSurfaceMeshDistanceBound->text().toDouble();

    // create quadric
    Predicate_G_3_Z g3;

    switch (storedPredicate->variantPredicate.type)
    {
    case VariantPredicate::None:
        assert(0);
        break;

    case VariantPredicate::Halfspace:
        g3 = storedPredicate->variantPredicate.h3;
        break;

    case VariantPredicate::Screw:
        g3 = storedPredicate->variantPredicate.s3;
        break;

    case VariantPredicate::Generic:
        g3 = storedPredicate->variantPredicate.g3;
        break;
    }

    storedPredicate->quadric = Spin_quadric_3_Z(g3);

    // mesh predicate to a triangle soup
    storedPredicate->leftSoup.reset(new Mesh_smooth_triangle_list_3_Z());
    storedPredicate->rightSoup.reset(new Mesh_smooth_triangle_list_3_Z());

    if (!ui->checkBoxSkipQuadricsMeshing->isChecked())
    {
        convertQuadricToTriangleSoups(
            storedPredicate->quadric,
            storedPredicate->angularBound,
            storedPredicate->radiusBound,
            storedPredicate->distanceBound,
            storedPredicate->leftSoup,
            storedPredicate->rightSoup);

        // display boh triangle soups
        m_cspaceView->addTriangleList(storedPredicate->leftSoup, Qt::red);
        m_cspaceView->addTriangleList(storedPredicate->rightSoup, Qt::red);

        m_cspaceView->updateGL();
    }

    // add predicate to ui
    ExtendedTreeWidgetItem *rootItem = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::TriangleSoup);
    rootItem->setToolTip(0, QString::fromStdString(storedPredicate->quadric.to_string()));

    ExtendedTreeWidgetItem *itemLeft = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::TriangleSoup);
    itemLeft->setCheckState(0, Qt::Checked);
    itemLeft->setText(0, tr("Left (negative)"));
    itemLeft->setTriangleSoup(storedPredicate->leftSoup);
    rootItem->addChild(itemLeft);

    ExtendedTreeWidgetItem *itemRight = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::TriangleSoup);
    itemRight->setCheckState(0, Qt::Checked);
    itemRight->setText(0, tr("Right (positive)"));
    itemRight->setTriangleSoup(storedPredicate->rightSoup);
    rootItem->addChild(itemRight);

    std::ostringstream strPredicate;

    switch (storedPredicate->variantPredicate.type)
    {
    case VariantPredicate::None:
        assert(0);
        break;

    case VariantPredicate::Halfspace:
        strPredicate << "H3: " << storedPredicate->variantPredicate.h3;
        break;

    case VariantPredicate::Screw:
        strPredicate << "S3: " << storedPredicate->variantPredicate.s3;
        break;

    case VariantPredicate::Generic:
        strPredicate << "G3: " << storedPredicate->variantPredicate.g3;
        break;
    }

    rootItem->setText(0, QString::fromStdString(strPredicate.str()));
    ui->treeWidgetConstraints->addTopLevelItem(rootItem);
    rootItem->setExpanded(true);

    // store the predicate
    m_storedPredicates.push_back(storedPredicate);

    // add induced qsics
    showAddInducedQsicsDialog(storedPredicate);

    // update summary
    ui->labelSummary->setText(
                QString("%1 predicates, %2 qsics, %3 qsips").arg(m_storedPredicates.size()).arg(m_storedQsics.size()).arg(m_storedQsips.size()));
}

void MainWindow::showAddInducedQsicsDialog(StoredPredicatePtr storedPredicate)
{
    // if there is only one stored predicate, there is no point in bothering
    // the user about induced qsics
    if (m_storedPredicates.size() <= 1)
        return;

    if (ui->checkBoxSkipAddingQSICs->isChecked())
        return;

    if (ui->checkBoxDoNotAskAboutInduced->isChecked() ||
        QMessageBox::question(this,
                              tr("Induced QSIC-s"),
                              tr("Do you want to add all induced QSIC-s in scene?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::Yes) == QMessageBox::Yes)
    {
        // add all induced QSICs
        addInducedQsics(storedPredicate);
    }
}

void MainWindow::showAddInducedQsipsDialog(StoredQsicPtr storedQsic)
{
    // if there are only two stored predicates, there is no point in bothering
    // the user about induced qsips
    if (m_storedPredicates.size() <= 2)
        return;

    if (ui->checkBoxSkipAddingQSIPs->isChecked())
        return;

    if (ui->checkBoxDoNotAskAboutInduced->isChecked() ||
        QMessageBox::question(this,
                              tr("Induced QSIP-s"),
                              tr("Do you want to add all induced QSIP-s in scene?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::Yes) == QMessageBox::Yes)
    {
        // add all induced QSIPs
        addInducedQsips(storedQsic);
    }
}

void MainWindow::on_actionAddPredicate_triggered()
{
    // default is H3
    showAddPredicateDialog(VariantPredicate::Halfspace);
}

void MainWindow::on_pushButtonAddPredicate_clicked()
{
    // emit action
    emit on_actionAddPredicate_triggered();
}

void MainWindow::on_pushButtonRemovePredicate_clicked()
{
}

void MainWindow::convertQuadricToTriangleSoups(
        const Spin_quadric_3_Z &quadric,
        const double angular_bound,
        const double radius_bound,
        const double distance_bound,
        Mesh_smooth_triangle_list_3_Z_ptr &left,
        Mesh_smooth_triangle_list_3_Z_ptr &right)
{
    Spin_quadric_mesh_3_Z mesher(quadric);

    mesher.mesh_triangle_soup(std::back_inserter(*left),
                              std::back_inserter(*right),
                              angular_bound,
                              radius_bound,
                              distance_bound);
}

void MainWindow::on_treeWidgetConstraints_itemChanged(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    ExtendedTreeWidgetItem *extendedItem = static_cast<ExtendedTreeWidgetItem *>(item);

    switch (extendedItem->type())
    {
    case ExtendedTreeWidgetItem::TriangleSoup:
        // Check state
        m_cspaceView->setTriangleListVisible(extendedItem->triangleSoup(), item->checkState(0) == Qt::Checked);
        m_cspaceView->updateGL();
        break;

    case ExtendedTreeWidgetItem::SpinList:
        // Check state
        m_cspaceView->setSpinListVisible(extendedItem->spinList(), item->checkState(0) == Qt::Checked);
        m_cspaceView->updateGL();
        break;

    case ExtendedTreeWidgetItem::Spin:
        // Check state
        m_cspaceView->setSpinVisible(extendedItem->spin(), item->checkState(0) == Qt::Checked);
        m_cspaceView->updateGL();
        break;

    case ExtendedTreeWidgetItem::None:
        break;
    }
}

void MainWindow::addInducedQsics(StoredPredicatePtr storedPredicate)
{
    double radiusBound = ui->lineEditCurveMeshRadiusBound->text().toDouble();

    // Add all qsics created from the intersection of  a given quadric and all existing
    foreach (const StoredPredicatePtr &iteratorPredicate, m_storedPredicates)
    {
        // do not self-intersect
        if (storedPredicate == iteratorPredicate)
            continue;

        // add spin-QSICs to ui
        ExtendedTreeWidgetItem *rootItem = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::TriangleSoup);

        std::ostringstream description;
        description << "spin-QSIC";

        rootItem->setText(0, QString::fromStdString(description.str()));
        ui->treeWidgetConstraints->addTopLevelItem(rootItem);
        rootItem->setExpanded(true);

        // intersect
        Spin_quadric_3_Z q1 = iteratorPredicate->quadric;
        Spin_quadric_3_Z q2 = storedPredicate->quadric;

        // stored qsic
        StoredQsicPtr storedQsic(new StoredQsic());

        // create spin-QSIC
        storedQsic->qsic.reset(new Spin_qsic_3_Z(q1, q2));

        // mesh spin-QSIC
        Spin_qsic_mesh_3_Z mesher(*storedQsic->qsic);

        // each component
        for (size_t component = 0; component != mesher.size_of_components(); ++component)
        {
            // FIXME: if the component is not one dimensional, abandon it
            if (storedQsic->qsic->component_dimension(component) != 1)
                continue;

            // evaluate curve
            Qsic_spin_list_3_Z_ptr spinList(new Qsic_spin_list_3_Z());

            if (!ui->checkBoxSkipQSICsMeshing->isChecked())
            {
                mesher.mesh_component(*spinList, component, radiusBound);
                m_cspaceView->addSpinList(spinList);
            }

            // positive
            storedQsic->spinLists.push_back(spinList);

            ExtendedTreeWidgetItem *item = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::SpinList);
            item->setCheckState(0, Qt::Checked);
            item->setText(0, tr("Component #") + QString::number(component) + tr(" (positive)"));
            item->setToolTip(0, QString::fromStdString(storedQsic->qsic->component_to_string(component)));
            item->setSpinList(spinList);
            rootItem->addChild(item);

            // negative
            Qsic_spin_list_3_Z_ptr negSpinList(new Qsic_spin_list_3_Z());

            if (!ui->checkBoxSkipQSICsMeshing->isChecked())
            {
                foreach (const Qsic_spin_3_Z &spin, *spinList)
                    negSpinList->push_back(-spin);

                m_cspaceView->addSpinList(negSpinList);
            }

            storedQsic->spinLists.push_back(negSpinList);

            item = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::SpinList);
            item->setCheckState(0, Qt::Checked);
            item->setText(0, tr("Component #") + QString::number(component) + tr(" (negative)"));
            item->setToolTip(0, QString::fromStdString(storedQsic->qsic->component_to_string(component)));
            item->setSpinList(negSpinList);
            rootItem->addChild(item);
        }

        // store the qsic
        m_storedQsics.push_back(storedQsic);

        // add induced qsips
        showAddInducedQsipsDialog(storedQsic);
    }

    // update view all at once
    m_cspaceView->updateGL();
}

void MainWindow::addInducedQsips(StoredQsicPtr storedQsic)
{
    // add self-intersections of this qsic, especially in the case of four lines

    // FIXME!!!
    // !!!

    // Add all qsips created from the intersection of a quadric with a qsic
    foreach (const StoredPredicatePtr &iteratorPredicate, m_storedPredicates)
    {
        // add spin-QSIPs to ui
        ExtendedTreeWidgetItem *rootItem = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::Spin);

        std::ostringstream description;
        description << "spin-QSIP";

        rootItem->setText(0, QString::fromStdString(description.str()));
        ui->treeWidgetConstraints->addTopLevelItem(rootItem);
        rootItem->setExpanded(true);

        // intersect
        Spin_quadric_3_Z q = iteratorPredicate->quadric;
        Spin_qsic_3_Z c = *storedQsic->qsic;

        // stored qsip
        StoredQsipPtr storedQsip(new StoredQsip());

        // create spin-QSIP
        storedQsip->qsip.reset(new Spin_qsip_3_Z(q, c));

        // mesh spin-QSIP
        Spin_qsip_mesh_3_Z mesher(*storedQsip->qsip);

        // each component
        for (size_t index = 0; index != mesher.size_of_points(); ++index)
        {
            // evaluate point
            Qsip_spin_3_Z_ptr spin(new Qsip_spin_3_Z());

            if (!ui->checkBoxSkipQSIPsMeshing->isChecked())
            {
                mesher.mesh_point(*spin, index);
                m_cspaceView->addSpin(spin);
            }

            // positive
            storedQsip->spin = spin;

            ExtendedTreeWidgetItem *item = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::Spin);
            item->setCheckState(0, Qt::Checked);
            item->setText(0, tr("Spin #") + QString::number(index) + tr(" (positive)"));
            item->setSpin(spin);
            rootItem->addChild(item);

            // negative
            Qsip_spin_3_Z_ptr negSpin(new Qsip_spin_3_Z());

            if (!ui->checkBoxSkipQSIPsMeshing->isChecked())
            {
                *negSpin = -*spin;
                m_cspaceView->addSpin(negSpin);
            }

            storedQsip->negSpin = negSpin;

            item = new ExtendedTreeWidgetItem(ExtendedTreeWidgetItem::Spin);
            item->setCheckState(0, Qt::Checked);
            item->setText(0, tr("Spin #") + QString::number(index) + tr(" (negative)"));
            item->setSpin(negSpin);
            rootItem->addChild(item);
        }

        // store the qsic
        m_storedQsips.push_back(storedQsip);
    }

    // update view all at once
    m_cspaceView->updateGL();
}
*/
void MainWindow::on_actionQuit_triggered()
{
    // close main form
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::on_actionBenchmark_triggered()
{
    // just show benchmark window, which do it all
    BenchmarkDialog benchmarkDialog(this);
    benchmarkDialog.exec();
}

void MainWindow::toggleFullScreen()
{
    if (!isFullScreen())
        setWindowState(Qt::WindowFullScreen);
    else
        setWindowState(Qt::WindowNoState);
}

/*
void MainWindow::setExtendedItemsVisible(ExtendedTreeWidgetItem::Type type, bool visible)
{
    int count = ui->treeWidgetConstraints->topLevelItemCount();

    for (int index = 0; index < count; ++index)
    {
        QTreeWidgetItem* topLevelItem = ui->treeWidgetConstraints->topLevelItem(index);

        int childrenCount = topLevelItem->childCount();

        for (int childIndex = 0; childIndex < childrenCount; ++childIndex)
        {
            QTreeWidgetItem* child = topLevelItem->child(childIndex);
            ExtendedTreeWidgetItem *extendedChild = static_cast<ExtendedTreeWidgetItem *>(child);

            if (extendedChild->type() == type)
                extendedChild->setCheckState(0, visible ? Qt::Checked : Qt::Unchecked);
        }
    }
}

void MainWindow::on_pushButtonHideQuadrics_clicked()
{
    setExtendedItemsVisible(ExtendedTreeWidgetItem::TriangleSoup, false);
}

void MainWindow::on_pushButtonHideQsics_clicked()
{
    setExtendedItemsVisible(ExtendedTreeWidgetItem::SpinList, false);
}

void MainWindow::on_pushButtonHideQsips_clicked()
{
    setExtendedItemsVisible(ExtendedTreeWidgetItem::Spin, false);
}

void MainWindow::on_pushButtonShowQuadrics_clicked()
{
    setExtendedItemsVisible(ExtendedTreeWidgetItem::TriangleSoup, true);
}

void MainWindow::on_pushButtonShowQsics_clicked()
{
    setExtendedItemsVisible(ExtendedTreeWidgetItem::SpinList, true);
}

void MainWindow::on_pushButtonShowQsips_clicked()
{
    setExtendedItemsVisible(ExtendedTreeWidgetItem::Spin, true);
}

void MainWindow::on_actionLoadArrangement_triggered()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            tr("Open arrangement"),
            QDir::homePath(),
            tr("Arrangement files (*.arr)"));

    if (fileName.isEmpty())
        return;

    std::ifstream file(fileName.toStdString().c_str());

    if (!file.fail())
    {
        // clear scene
        clearScene();

        // read until end
        std::string type;

        while (file >> type)
        {
            VariantPredicate::Type predicateType;

            if (type == "H")
                predicateType = VariantPredicate::Halfspace;
            else if (type == "S")
                predicateType = VariantPredicate::Screw;
            else if (type == "G")
                predicateType = VariantPredicate::Generic;
            else
            {
                QMessageBox::warning(this,
                                     tr("Load arrangement"),
                                     tr("Invalid predicate type"),
                                     QMessageBox::Yes);
                return;
            }

            switch (predicateType)
            {
                case VariantPredicate::None:
                    assert(0);
                    break;

                case VariantPredicate::Halfspace:
                {
                    Z bx, by, bz, pa, pb, pc, pd;
                    file >> bx >> by >> bz >> pa >> pb >> pc >> pd;

                    if (file.fail())
                    {
                        QMessageBox::warning(this,
                                             tr("Load arrangement"),
                                             tr("Invalid H3 predicate data"),
                                             QMessageBox::Yes);
                        return;
                    }

                    // add to scene
                    addPredicateToScene(
                        VariantPredicate(
                            Predicate_H_3_Z(
                                Vector_3_Z(bx, by, bz),
                                Plane_3_Z(pa, pb, pc, pd))));
                }
                break;

                case VariantPredicate::Screw:
                {
                    Z ax, ay, az, bx, by, bz, kx, ky, kz, lx, ly, lz;

                    file >> ax >> ay >> az
                         >> bx >> by >> bz
                         >> kx >> ky >> kz
                         >> lx >> ly >> lz;

                    if (file.fail())
                    {
                        QMessageBox::warning(this,
                                             tr("Load arrangement"),
                                             tr("Invalid S3 predicate data"),
                                             QMessageBox::Yes);
                        return;
                    }

                    // add to scene
                    addPredicateToScene(
                        VariantPredicate(
                            Predicate_S_3_Z(
                                Vector_3_Z(ax, ay, az),
                                Vector_3_Z(bx, by, bz),
                                Vector_3_Z(kx, ky, kz),
                                Vector_3_Z(lx, ly, lz))));
                }
                break;

                case VariantPredicate::Generic:
                {
                    Z ax, ay, az, bx, by, bz, kx, ky, kz, lx, ly, lz, c;

                    file >> ax >> ay >> az
                         >> bx >> by >> bz
                         >> kx >> ky >> kz
                         >> lx >> ly >> lz
                         >> c;

                    if (file.fail())
                    {
                        QMessageBox::warning(this,
                                             tr("Load arrangement"),
                                             tr("Invalid G3 predicate data"),
                                             QMessageBox::Yes);
                        return;
                    }

                    // add to scene
                    addPredicateToScene(
                        VariantPredicate(
                            Predicate_G_3_Z(
                                Vector_3_Z(ax, ay, az),
                                Vector_3_Z(bx, by, bz),
                                Vector_3_Z(kx, ky, kz),
                                Vector_3_Z(lx, ly, lz),
                                c)));
                }
                break;
            }
        }
    }
}

void MainWindow::on_actionSaveArrangement_triggered()
{
    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            tr("Save arrangement"),
            QDir::homePath(),
            tr("Arrangement files (*.arr)"));

    if (fileName.isEmpty())
        return;

    std::ofstream file(fileName.toStdString().c_str());

    if (!file.fail())
    {
        // save predicates, one per line
        foreach (StoredPredicatePtr predicate, m_storedPredicates)
        {
            switch (predicate->variantPredicate.type)
            {
            case VariantPredicate::None:
                assert(0);
                break;

            case VariantPredicate::Halfspace:
                file
                    << "H" << "   "
                    << predicate->variantPredicate.h3.b().x() << " "
                    << predicate->variantPredicate.h3.b().y() << " "
                    << predicate->variantPredicate.h3.b().z() << "  "
                    << predicate->variantPredicate.h3.p().a() << " "
                    << predicate->variantPredicate.h3.p().b() << " "
                    << predicate->variantPredicate.h3.p().c() << " "
                    << predicate->variantPredicate.h3.p().d()
                    << "\n";
                break;

            case VariantPredicate::Screw:
                file
                    << "S" << "   "
                    << predicate->variantPredicate.s3.a().x() << " "
                    << predicate->variantPredicate.s3.a().y() << " "
                    << predicate->variantPredicate.s3.a().z() << "  "
                    << predicate->variantPredicate.s3.b().x() << " "
                    << predicate->variantPredicate.s3.b().y() << " "
                    << predicate->variantPredicate.s3.b().z() << "  "
                    << predicate->variantPredicate.s3.k().x() << " "
                    << predicate->variantPredicate.s3.k().y() << " "
                    << predicate->variantPredicate.s3.k().z() << "  "
                    << predicate->variantPredicate.s3.l().x() << " "
                    << predicate->variantPredicate.s3.l().y() << " "
                    << predicate->variantPredicate.s3.l().z()
                    << "\n";
                break;

            case VariantPredicate::Generic:
                file
                    << "G" << "   "
                    << predicate->variantPredicate.g3.a().x() << " "
                    << predicate->variantPredicate.g3.a().y() << " "
                    << predicate->variantPredicate.g3.a().z() << "  "
                    << predicate->variantPredicate.g3.b().x() << " "
                    << predicate->variantPredicate.g3.b().y() << " "
                    << predicate->variantPredicate.g3.b().z() << "  "
                    << predicate->variantPredicate.g3.k().x() << " "
                    << predicate->variantPredicate.g3.k().y() << " "
                    << predicate->variantPredicate.g3.k().z() << "  "
                    << predicate->variantPredicate.g3.l().x() << " "
                    << predicate->variantPredicate.g3.l().y() << " "
                    << predicate->variantPredicate.g3.l().z() << " "
                    << predicate->variantPredicate.g3.c()
                    << "\n";
                break;
            }
        }
    }
}

void MainWindow::on_actionCloseArrangement_triggered()
{
    clearScene();
}

void MainWindow::clearScene()
{
    // clear solution
    ui->treeWidgetConstraints->clear();

    // clear renderview
    m_cspaceView->removeAllPredicates();

    // clear predicates
    m_storedPredicates.clear();
    m_storedQsics.clear();
    m_storedQsips.clear();
}

void MainWindow::on_actionTriangleIntersection_triggered()
{
    TriangleIntersectionDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // add triangles
    std::pair<Mesh_smooth_triangle_3_Z, Mesh_smooth_triangle_3_Z> trianglePair = dialog.trianglePair();

    Mesh_smooth_triangle_list_3_Z_ptr triangleList(new Mesh_smooth_triangle_list_3_Z());
    triangleList->push_back(trianglePair.first);
    triangleList->push_back(trianglePair.second);

    m_sceneView->addTriangleList(triangleList, Qt::red);

    // get predicate list from triangle triangle intersection test
    std::vector<VariantPredicate> predicates = dialog.predicateList();

    foreach (VariantPredicate predicate, predicates)
        addPredicateToScene(predicate);
}

*/
