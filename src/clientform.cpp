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
#include "clientform.h"
#include "ispoweroftwo.h"
#include "renderview.h"
#include "renderviewarcballcamera.h"
#include "renderviewflycamera.h"
#include "renderviewautocamera.h"
#include "sceneobjectdialog.h"
#include "configurationobjectdialog.h"
#include "rasterconfigurationspace.h"
#include "cellconfigurationspace.h"
#include "exactconfigurationspace.h"
#include "variantpredicatedialog.h"
#include "qlog4cxx.h"
#include "ui_clientform.h"
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QTimer>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QCursor>
#include <QIcon>
#include <QMenu>
#include <QTimer>
#include <algorithm>
#include <cassert>
#include <cstdlib>

namespace // anonymous
{
static QString boolToString(bool value)
{
    return value ? QObject::tr("yes") : QObject::tr("no");
}

QQuaternion quaternionFromEulerAngles(double roll, double pitch, double yaw)
{
    // from www.euclideanspace.com
    double c1 = std::cos(yaw / 2.0);
    double s1 = std::sin(yaw / 2.0);
    double c2 = std::cos(pitch / 2.0);
    double s2 = std::sin(pitch / 2.0);
    double c3 = std::cos(roll / 2.0);
    double s3 = std::sin(roll / 2.0);
    double c1c2 = c1 * c2;
    double s1s2 = s1 * s2;

    double w = c1c2 * c3 - s1s2 * s3;
    double x = c1c2 * s3 + s1s2 * c3;
    double y = s1 * c2 * c3 + c1 * s2 * s3;
    double z = c1 * s2 * c3 - s1 * c2 * s3;

    return QQuaternion(w, x, y, z);
}

int calculateNumberOfFractionDigits(const QDecimal &decimal)
{
    QDecimal reducedDecimal = decimal.reduce();
    QByteArray buffer = reducedDecimal.toString();

    int dotIndex = buffer.indexOf('.');

    if (dotIndex == -1)
        return 0;

    return buffer.size() - 1 - dotIndex;
}

void updateMaximumFractionDigits(int &numberOfFractionDigits, const QDecimal &decimal)
{
    int fractionalDigits = calculateNumberOfFractionDigits(decimal);

    if (fractionalDigits > numberOfFractionDigits)
        numberOfFractionDigits = fractionalDigits;
}

Z decimalToZ(const QDecimal &decimal)
{
    QByteArray buffer = decimal.toString();

    int dotIndex = buffer.indexOf('.');

    if (dotIndex != -1)
        buffer[dotIndex] = '\0';
    else
        buffer.append('\0');

    Z result;
    string_to_bigint(buffer.data(), result);
    return result;
}

//const int MOTION_ANIMATION_TIME = 5000;
const int MAXIMUM_EXACT_TRUNCATION_DIGITS = 32;
} // namespace anonymous

ClientForm::ClientForm(QWidget *parent) :
    QWidget(parent),
    m_configurationObjectPopupRow(-1),
    m_motionTimer(0),
    ui(new Ui::ClientForm)
{
    ui->setupUi(this);

    // setup a shared two GL views
    m_widgetSceneView = new RenderView();
    ui->widgetSceneViewContainer->layout()->addWidget(m_widgetSceneView);

    m_widgetConfigurationView = new RenderView(0, m_widgetSceneView);
    ui->widgetConfigurationViewContainer->layout()->addWidget(m_widgetConfigurationView);

    m_widgetSceneView->setCaption("scene space");
    m_widgetConfigurationView->setCaption("c-space projection");

    connect(m_widgetSceneView, SIGNAL(toggleFullScreenTriggered()), this, SLOT(toggleSceneFullScreenTriggered()));
    connect(m_widgetConfigurationView, SIGNAL(toggleFullScreenTriggered()), this, SLOT(toggleConfigurationFullScreenTriggered()));

    // camera
    setSceneCamera(CT_ArcBall);

    // setup toolbox menu
    QMenu *toolButtonMenu = new QMenu();
    QAction *actionRasterGraphTexture3D = new QAction("Raster graph (Texture3D)", toolButtonMenu);
    connect(actionRasterGraphTexture3D, SIGNAL(triggered()), this, SLOT(convertToRasterGraphTexture3DTriggered()));
    toolButtonMenu->addAction(actionRasterGraphTexture3D);
    QAction *actionRasterGraphGaussianSplatter = new QAction("Raster graph (Gaussian Splatter)", toolButtonMenu);
    connect(actionRasterGraphGaussianSplatter, SIGNAL(triggered()), this, SLOT(convertToRasterGraphGaussianSplatterTriggered()));
    toolButtonMenu->addAction(actionRasterGraphGaussianSplatter);
    QAction *actionCellGraphGaussianSplatter = new QAction("Cell graph (Gaussian Splatter)", toolButtonMenu);
    connect(actionCellGraphGaussianSplatter, SIGNAL(triggered()), this, SLOT(convertToCellGraphGaussianSplatterTriggered()));
    toolButtonMenu->addAction(actionCellGraphGaussianSplatter);
    QAction *actionExactGraph = new QAction("Exact graph", toolButtonMenu);
    connect(actionExactGraph, SIGNAL(triggered()), this, SLOT(convertToExactGraphTriggered()));
    toolButtonMenu->addAction(actionExactGraph);

    ui->toolButtonConvert->setMenu(toolButtonMenu);

    // attach to logger
    connect(QLog4cxx::instance(), SIGNAL(logMessage(QString,QString,long long,QString)), this, SLOT(logMessage(QString,QString,long long,QString)));
}

ClientForm::~ClientForm()
{
    // detach from logger
    QLog4cxx::instance()->disconnect(this);

    // delete ui
    delete ui;
}

void ClientForm::setSceneCamera(ClientForm::CameraType type)
{
    switch (type)
    {
    case CT_ArcBall:
        ui->toolButtonSceneCameraArcBall->setChecked(true);
        ui->toolButtonSceneCameraFlying->setChecked(false);
        ui->toolButtonSceneCameraAutomatic->setChecked(false);
        m_widgetSceneView->setRenderViewCamera(new RenderViewArcBallCamera());
        break;

    case CT_Flying:
        ui->toolButtonSceneCameraArcBall->setChecked(false);
        ui->toolButtonSceneCameraFlying->setChecked(true);
        ui->toolButtonSceneCameraAutomatic->setChecked(false);
        m_widgetSceneView->setRenderViewCamera(new RenderViewFlyCamera());
        break;

    case CT_Automatic:
        ui->toolButtonSceneCameraArcBall->setChecked(false);
        ui->toolButtonSceneCameraFlying->setChecked(false);
        ui->toolButtonSceneCameraAutomatic->setChecked(true);

        // setup automatic camera
        {
            RenderViewAutoCamera *camera = new RenderViewAutoCamera();
            connect(camera, SIGNAL(needsUpdate()), m_widgetSceneView, SLOT(updateGL()));
            m_widgetSceneView->setRenderViewCamera(camera);
        }
        break;
    }
}

void ClientForm::setConfigurationCamera(ClientForm::CameraType type)
{
    switch (type)
    {
    case CT_ArcBall:
        ui->toolButtonConfigurationCameraArcBall->setChecked(true);
        ui->toolButtonConfigurationCameraFlying->setChecked(false);
        ui->toolButtonConfigurationCameraAutomatic->setChecked(false);
        m_widgetConfigurationView->setRenderViewCamera(new RenderViewArcBallCamera());
        break;

    case CT_Flying:
        ui->toolButtonConfigurationCameraArcBall->setChecked(false);
        ui->toolButtonConfigurationCameraFlying->setChecked(true);
        ui->toolButtonConfigurationCameraAutomatic->setChecked(false);
        m_widgetConfigurationView->setRenderViewCamera(new RenderViewFlyCamera());
        break;

    case CT_Automatic:
        ui->toolButtonConfigurationCameraArcBall->setChecked(false);
        ui->toolButtonConfigurationCameraFlying->setChecked(false);
        ui->toolButtonConfigurationCameraAutomatic->setChecked(true);

        // setup automatic camera
        {
            RenderViewAutoCamera *camera = new RenderViewAutoCamera();
            connect(camera, SIGNAL(needsUpdate()), m_widgetConfigurationView, SLOT(updateGL()));
            m_widgetConfigurationView->setRenderViewCamera(camera);
        }
        break;
    }
}

size_t ClientForm::selectRasterResolution()
{
    bool ok = false;

    size_t resolution;

    do
    {
        resolution = static_cast<size_t>(QInputDialog::getInt(this,
                                         QObject::tr("Select resolution"),
                                         QObject::tr("Select raster resolution"),
                                         128,
                                         1,
                                         256,
                                         1,
                                         &ok));

        if (!ok)
            return 0;
    }
    while (!isPowerOfTwo(resolution));

    return resolution;
}

size_t ClientForm::selectCellSampleCount()
{
    bool ok = false;

    size_t sampleCount = static_cast<size_t>(QInputDialog::getInt(this,
                                             QObject::tr("Select sample count"),
                                             QObject::tr("Select cell graph sample count"),
                                             10000,
                                             1,
                                             5000000,
                                             1,
                                             &ok));

    if (!ok)
        return 0;

    return sampleCount;
}

void ClientForm::on_toolButtonSceneCameraArcBall_clicked()
{
    setSceneCamera(CT_ArcBall);
}

void ClientForm::on_toolButtonSceneCameraFlying_clicked()
{
    setSceneCamera(CT_Flying);
}

void ClientForm::on_toolButtonSceneCameraAutomatic_clicked()
{
    setSceneCamera(CT_Automatic);
}

void ClientForm::on_toolButtonConfigurationCameraArcBall_clicked()
{
    setConfigurationCamera(CT_ArcBall);
}

void ClientForm::on_toolButtonConfigurationCameraFlying_clicked()
{
    setConfigurationCamera(CT_Flying);
}

void ClientForm::on_toolButtonConfigurationCameraAutomatic_clicked()
{
    setConfigurationCamera(CT_Automatic);
}

void ClientForm::on_toolButtonOpenSceneSph_clicked()
{
    // import sphere tree
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            tr("Open sphere tree"),
            QString(),
            tr("Sphere tree files (*.sph)"));

    if (fileName.isEmpty())
        return;

    SceneObjectPtr object = SceneObject::loadFromSphereTree(fileName.toStdString().c_str(), this);

    if (!object)
        return (void)QMessageBox::warning(this, tr("Open file"), tr("Failed to load file!"), QMessageBox::Ok);

    // register
    object->setColor(m_widgetSceneView->nextSuggestedColor());
    addSceneObject(object, fileName.toStdString().c_str());
}

void ClientForm::addSceneObject(SceneObjectPtr object, const QString &fileName)
{
    // register object
    m_sceneObjects.push_back(object);

    // icon
    QIcon icon;
    QString info;
    QString type;

    // display object
    switch (object->type())
    {
    case SceneObject::Type_DecimalBallList:
        m_widgetSceneView->addDecimalBallList(object->decimalBallList(), object->color());
        icon = QIcon(":/resource/img/ball.png");
        info = QString("file: %1, balls: %2").arg(QFileInfo(fileName).fileName(), QString::number(object->decimalBallList()->size()));
        break;

    case SceneObject::Type_DecimalTriangleList:
        m_widgetSceneView->addDecimalTriangleList(object->decimalTriangleList(), object->color());
        icon = QIcon(":/resource/img/tritri.png");
        info = QString("file: %1, triangles: %2").arg(QFileInfo(fileName).fileName(), QString::number(object->decimalTriangleList()->size()));
        break;
    }

    type = object->typeString();

    // show in table
    int row = ui->tableWidgetSceneObjects->rowCount();

    ui->tableWidgetSceneObjects->insertRow(row);

    QTableWidgetItem *item;

    item = new QTableWidgetItem(icon, type);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableWidgetSceneObjects->setItem(row, 0, item);

    item = new QTableWidgetItem(boolToString(object->isVisible()));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidgetSceneObjects->setItem(row, 1, item);

    item = new QTableWidgetItem(boolToString(object->isRotating()));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidgetSceneObjects->setItem(row, 2, item);

    item = new QTableWidgetItem(info);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableWidgetSceneObjects->setItem(row, 3, item);

    ui->tableWidgetSceneObjects->item(row, 0)->setBackgroundColor(object->color());

    ui->tableWidgetSceneObjects->resizeRowToContents(row);
    ui->tableWidgetSceneObjects->resizeColumnsToContents();
}

void ClientForm::addConfigurationObject(ConfigurationObjectPtr object, const QString &fileName, const QIcon &icon, const QString &extraInfo)
{
    // register object
    m_configurationObjects.push_back(object);

    // icon
    QString info;
    QString type;

    // display object
    switch (object->type())
    {
    case ConfigurationObject::Type_RasterConfigurationSpace:
        m_widgetConfigurationView->addConfigurationSpace(object->rasterConfigurationSpace());
        break;

    case ConfigurationObject::Type_CellConfigurationSpace:
        m_widgetConfigurationView->addConfigurationSpace(object->cellConfigurationSpace());
        break;

    case ConfigurationObject::Type_ExactConfigurationSpace:
        m_widgetConfigurationView->addConfigurationSpace(object->exactConfigurationSpace());
        break;

    case ConfigurationObject::Type_Route:
        m_widgetConfigurationView->addConfigurationSpace(object->sampledRoute());
        break;
    }

    info = QString("file: %1").arg(QFileInfo(fileName).fileName()) + extraInfo;

    type = object->typeString();

    // show in table
    int row = ui->tableWidgetConfigurationObjects->rowCount();

    ui->tableWidgetConfigurationObjects->insertRow(row);

    QTableWidgetItem *item;

    item = new QTableWidgetItem(icon, type);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableWidgetConfigurationObjects->setItem(row, 0, item);

    item = new QTableWidgetItem(boolToString(object->isVisible()));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidgetConfigurationObjects->setItem(row, 1, item);

    item = new QTableWidgetItem(info);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableWidgetConfigurationObjects->setItem(row, 2, item);

    ui->tableWidgetConfigurationObjects->resizeRowToContents(row);
    ui->tableWidgetConfigurationObjects->resizeColumnsToContents();
}

void ClientForm::on_toolButtonOpenSceneDir_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open scene directory"), tr("."));

    if (directory.isEmpty())
        return;

    std::pair<SceneObjectPtr, SceneObjectPtr> objects = SceneObject::loadFromDirectory(directory.toStdString().c_str(), this);

    if (!objects.first || !objects.second)
        return (void)QMessageBox::warning(this, tr("Open directory"), tr("Failed to load directory!"), QMessageBox::Ok);

    // register
    objects.first->setColor(m_widgetSceneView->nextSuggestedColor());
    addSceneObject(objects.first, directory.toStdString().c_str());

    objects.second->setColor(m_widgetSceneView->nextSuggestedColor());
    addSceneObject(objects.second, directory.toStdString().c_str());
}

void ClientForm::on_labelSceneObjectDelete_linkActivated(const QString &link)
{
    Q_UNUSED(link);

    QList<QTableWidgetItem *> indexes = ui->tableWidgetSceneObjects->selectedItems();

    if (indexes.size() != ui->tableWidgetSceneObjects->columnCount())
        return;

    int row = indexes[0]->row();

    // remove it
    removeSceneObject(row);
}

void ClientForm::on_labelSceneObjectClear_linkActivated(const QString &link)
{
    Q_UNUSED(link);

    // TODO: optimize
    while (!m_sceneObjects.empty())
        removeSceneObject(0);
}

void ClientForm::removeSceneObject(int row)
{
    SceneObjectPtr object = m_sceneObjects[row];

    // remove display object
    switch (object->type())
    {
    case SceneObject::Type_DecimalBallList:
        m_widgetSceneView->removeDecimalBallList(object->decimalBallList());
        break;

    case SceneObject::Type_DecimalTriangleList:
        m_widgetSceneView->removeDecimalTriangleList(object->decimalTriangleList());
        break;
    }

    // remove row from table
    ui->tableWidgetSceneObjects->removeRow(row);

    // remove from model
    m_sceneObjects.erase(m_sceneObjects.begin() + row);
}

void ClientForm::removeConfigurationObject(int row)
{
    ConfigurationObjectPtr object = m_configurationObjects[row];

    // remove display object
    switch (object->type())
    {
    case ConfigurationObject::Type_RasterConfigurationSpace:
        m_widgetConfigurationView->removeConfigurationSpace(object->rasterConfigurationSpace());
        break;

    case ConfigurationObject::Type_CellConfigurationSpace:
        m_widgetConfigurationView->removeConfigurationSpace(object->cellConfigurationSpace());
        break;

    case ConfigurationObject::Type_ExactConfigurationSpace:
        m_widgetConfigurationView->removeConfigurationSpace(object->exactConfigurationSpace());
        break;

    case ConfigurationObject::Type_Route:
        m_widgetConfigurationView->removeConfigurationSpace(object->sampledRoute());
        break;
    }

    // remove row from table
    ui->tableWidgetConfigurationObjects->removeRow(row);

    // remove from model
    m_configurationObjects.erase(m_configurationObjects.begin() + row);
}

bool ClientForm::checkSceneObjects(SceneObject::Type *outType)
{
    // scene must not be empty
    if (m_sceneObjects.empty())
    {
        QMessageBox::warning(this, tr("Empty scene"), tr("The scene must not be empty!"), QMessageBox::Ok);
        return false;
    }

    // all objects must be balls or triangles
    SceneObject::Type type = m_sceneObjects.front()->type();

    for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin(); sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
    {
        if ((*sceneObjectIterator)->type() != type)
        {
            QMessageBox::warning(this, tr("Invalid scene"), tr("The scene must contain spheres or triangles only!"), QMessageBox::Ok);
            return false;
        }
    }

    *outType = type;
    return true;
}

void ClientForm::convertToRasterGraphTexture3DTriggered()
{
    // show console
    clearLogMessages();
    ui->tabWidgetConfiguration->setCurrentIndex(3);

    // check scene
    SceneObject::Type type;

    if (!checkSceneObjects(&type))
        return;

    // create configuration space from current scene
    createRasterConfigurationSpace(type, VolumeRendererType_Texture3D);
}

void ClientForm::convertToRasterGraphGaussianSplatterTriggered()
{
    // show console
    clearLogMessages();
    ui->tabWidgetConfiguration->setCurrentIndex(3);

    // check scene
    SceneObject::Type type;

    if (!checkSceneObjects(&type))
        return;

    // create configuration space from current scene
    createRasterConfigurationSpace(type, VolumeRendererType_GaussianSplatter);
}

void ClientForm::convertToCellGraphGaussianSplatterTriggered()
{
    // show console
    clearLogMessages();
    ui->tabWidgetConfiguration->setCurrentIndex(3);

    // check scene
    SceneObject::Type type;

    if (!checkSceneObjects(&type))
        return;

    // create configuration space from current scene
    createCellConfigurationSpace(type);
}

void ClientForm::convertToExactGraphTriggered()
{
    // show console
    clearLogMessages();
    ui->tabWidgetConfiguration->setCurrentIndex(3);

    // check scene
    SceneObject::Type type;

    if (!checkSceneObjects(&type))
        return;

    // create configuration space from current scene
    createExactConfigurationSpace(type);
}

void ClientForm::createRasterConfigurationSpace(SceneObject::Type type, VolumeRendererType volumeRendererType)
{
    // Note:
    // for the raster scenes, we will use triangulated or ball-only
    // scenes for an inexact kernel over R (double)
    RasterConfigurationSpacePtr rasterConfigurationSpace;

    // create scene
    switch (type)
    {
    case SceneObject::Type_DecimalBallList:
        {
            Ball_list_3_R movable;
            Ball_list_3_R obstacle;
            Ball_list_3_R *target;

            // accumulate scene and rotating object
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin();
                 sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                if ((*sceneObjectIterator)->isRotating())
                    target = &movable;
                else
                    target = &obstacle;

                for (DecimalBallList::const_iterator ballIterator = (*sceneObjectIterator)->decimalBallList()->begin();
                     ballIterator != (*sceneObjectIterator)->decimalBallList()->end(); ++ballIterator)
                {
                    target->push_back(Ball_3_R(Vector_3_R(ballIterator->center().x().toDouble(),
                                                          ballIterator->center().y().toDouble(),
                                                          ballIterator->center().z().toDouble()),
                                               ballIterator->radius().toDouble()));
                }
            }

            if (movable.empty() || obstacle.empty())
            {
                QMessageBox::warning(this, tr("Invalid scene"), tr("Neither movable nor obstacles can be empty!"), QMessageBox::Ok);
                return;
            }

            // get raster resolution
            size_t resolution = selectRasterResolution();

            if (!resolution)
                return;

            // create raster
            rasterConfigurationSpace.reset(
                new RasterConfigurationSpace(
                    RasterConfigurationSpaceTag<Spin_configuration_space_3::Raster_BB_R>(),
                    movable.begin(), movable.end(),
                    obstacle.begin(), obstacle.end(),
                    Spin_configuration_space_3::Raster_BB_R::Parameters(resolution),
                    volumeRendererType,
                    m_widgetConfigurationView));
        }
        break;

    case SceneObject::Type_DecimalTriangleList:
        {
            Triangle_list_3_R movable;
            Triangle_list_3_R obstacle;
            Triangle_list_3_R *target;

            // accumulate scene and rotating object
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin();
                 sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                if ((*sceneObjectIterator)->isRotating())
                    target = &movable;
                else
                    target = &obstacle;

                for (DecimalTriangleList::const_iterator triangleIterator = (*sceneObjectIterator)->decimalTriangleList()->begin();
                     triangleIterator != (*sceneObjectIterator)->decimalTriangleList()->end(); ++triangleIterator)
                {
                    target->push_back(Triangle_3_R(Point_3_R(triangleIterator->vertex(0).x().toDouble(),
                                                             triangleIterator->vertex(0).y().toDouble(),
                                                             triangleIterator->vertex(0).z().toDouble()),
                                                   Point_3_R(triangleIterator->vertex(1).x().toDouble(),
                                                             triangleIterator->vertex(1).y().toDouble(),
                                                             triangleIterator->vertex(1).z().toDouble()),
                                                   Point_3_R(triangleIterator->vertex(2).x().toDouble(),
                                                             triangleIterator->vertex(2).y().toDouble(),
                                                             triangleIterator->vertex(2).z().toDouble())));
                }
            }

            if (movable.empty() || obstacle.empty())
            {
                QMessageBox::warning(this, tr("Invalid scene"), tr("Neither movable nor obstacles can be empty!"), QMessageBox::Ok);
                return;
            }

            // get raster resolution
            size_t resolution = selectRasterResolution();

            if (!resolution)
                return;

            // create raster
            rasterConfigurationSpace.reset(
                new RasterConfigurationSpace(
                    RasterConfigurationSpaceTag<Spin_configuration_space_3::Raster_TT_R>(),
                    movable.begin(), movable.end(),
                    obstacle.begin(), obstacle.end(),
                    Spin_configuration_space_3::Raster_TT_R::Parameters(resolution),
                    volumeRendererType,
                    m_widgetConfigurationView));
        }
        break;
    }

    // add configuration space to view
    ConfigurationObjectPtr configurationObject(new ConfigurationObject(rasterConfigurationSpace));
    addConfigurationObject(configurationObject, "<generated>", QIcon(":/resource/img/recalculate.png"));
}

void ClientForm::createCellConfigurationSpace(SceneObject::Type type)
{
    // Note:
    // for the cell scenes, we will use triangulated or ball-only
    // scenes for an inexact kernel over R (double)
    CellConfigurationSpacePtr cellConfigurationSpace;

    // create scene
    switch (type)
    {
    case SceneObject::Type_DecimalBallList:
        {
            Ball_list_3_R movable;
            Ball_list_3_R obstacle;
            Ball_list_3_R *target;

            // accumulate scene and rotating object
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin();
                 sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                if ((*sceneObjectIterator)->isRotating())
                    target = &movable;
                else
                    target = &obstacle;

                for (DecimalBallList::const_iterator ballIterator = (*sceneObjectIterator)->decimalBallList()->begin();
                     ballIterator != (*sceneObjectIterator)->decimalBallList()->end(); ++ballIterator)
                {
                    target->push_back(Ball_3_R(Vector_3_R(ballIterator->center().x().toDouble(),
                                                          ballIterator->center().y().toDouble(),
                                                          ballIterator->center().z().toDouble()),
                                               ballIterator->radius().toDouble()));
                }
            }

            if (movable.empty() || obstacle.empty())
            {
                QMessageBox::warning(this, tr("Invalid scene"), tr("Neither movable nor obstacles can be empty!"), QMessageBox::Ok);
                return;
            }

            // get raster resolution
            size_t sampleCount = selectCellSampleCount();

            if (!sampleCount)
                return;

            // setup libcs config for cell graph
            CS::Config::set_neighbour_collect_algorithm(ui->comboBoxCellNeighbourCollectAlgorithm->currentIndex() + 1);

            // create raster
            cellConfigurationSpace.reset(
                new CellConfigurationSpace(
                    CellConfigurationSpaceTag<Spin_configuration_space_3::Cell_BB_R>(),
                    movable.begin(), movable.end(),
                    obstacle.begin(), obstacle.end(),
                    Spin_configuration_space_3::Cell_BB_R::Parameters(sampleCount),
                    m_widgetConfigurationView));
        }
        break;

    case SceneObject::Type_DecimalTriangleList:
        {
            Triangle_list_3_R movable;
            Triangle_list_3_R obstacle;
            Triangle_list_3_R *target;

            // accumulate scene and rotating object
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin();
                 sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                if ((*sceneObjectIterator)->isRotating())
                    target = &movable;
                else
                    target = &obstacle;

                for (DecimalTriangleList::const_iterator triangleIterator = (*sceneObjectIterator)->decimalTriangleList()->begin();
                     triangleIterator != (*sceneObjectIterator)->decimalTriangleList()->end(); ++triangleIterator)
                {
                    target->push_back(Triangle_3_R(Point_3_R(triangleIterator->vertex(0).x().toDouble(),
                                                             triangleIterator->vertex(0).y().toDouble(),
                                                             triangleIterator->vertex(0).z().toDouble()),
                                                   Point_3_R(triangleIterator->vertex(1).x().toDouble(),
                                                             triangleIterator->vertex(1).y().toDouble(),
                                                             triangleIterator->vertex(1).z().toDouble()),
                                                   Point_3_R(triangleIterator->vertex(2).x().toDouble(),
                                                             triangleIterator->vertex(2).y().toDouble(),
                                                             triangleIterator->vertex(2).z().toDouble())));
                }
            }

            if (movable.empty() || obstacle.empty())
            {
                QMessageBox::warning(this, tr("Invalid scene"), tr("Neither movable nor obstacles can be empty!"), QMessageBox::Ok);
                return;
            }

            // get raster resolution
            size_t sampleCount = selectCellSampleCount();

            if (!sampleCount)
                return;

            // create raster
            cellConfigurationSpace.reset(
                new CellConfigurationSpace(
                    CellConfigurationSpaceTag<Spin_configuration_space_3::Cell_TT_R>(),
                    movable.begin(), movable.end(),
                    obstacle.begin(), obstacle.end(),
                    Spin_configuration_space_3::Cell_TT_R::Parameters(sampleCount),
                    m_widgetConfigurationView));
        }
        break;
    }

    // add configuration space to view
    ConfigurationObjectPtr configurationObject(new ConfigurationObject(cellConfigurationSpace));
    addConfigurationObject(configurationObject, "<generated>", QIcon(":/resource/img/recalculate.png"));
}

void ClientForm::createExactConfigurationSpace(SceneObject::Type type)
{
    // meshing
    bool suppressQuadricMeshing = ui->checkBoxSkipQuadricsMeshing->isChecked();
    bool suppressQsicMeshing = ui->checkBoxSkipQSICsMeshing->isChecked();
    bool suppressQsipMeshing = ui->checkBoxSkipQSIPsMeshing->isChecked();

    // calculation
    bool suppressQsicCalculation = ui->checkBoxSkipAddingQSICs->isChecked();
    bool suppressQsipCalculation = ui->checkBoxSkipAddingQSIPs->isChecked();

    // options
    bool optionViewClipPlane = ui->checkBoxOptionViewClipPlane->isChecked();

    // Note:
    // for the exact scenes, we will use triangulated or ball-only
    // scenes for an EXACT kernel over Z
    ExactConfigurationSpacePtr exactConfigurationSpace;

    // find a common scene denominator to convert it to Z
    int maximumNumberOfFractionDigits = 0;

    switch (type)
    {
    case SceneObject::Type_DecimalBallList:
        {
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin();
                 sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                for (DecimalBallList::const_iterator ballIterator = (*sceneObjectIterator)->decimalBallList()->begin();
                     ballIterator != (*sceneObjectIterator)->decimalBallList()->end(); ++ballIterator)
                {
                    updateMaximumFractionDigits(maximumNumberOfFractionDigits, ballIterator->center().x());
                    updateMaximumFractionDigits(maximumNumberOfFractionDigits, ballIterator->center().y());
                    updateMaximumFractionDigits(maximumNumberOfFractionDigits, ballIterator->center().z());
                    updateMaximumFractionDigits(maximumNumberOfFractionDigits, ballIterator->radius());
                }
            }
        }
        break;

    case SceneObject::Type_DecimalTriangleList:
        {
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin(); sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                for (DecimalTriangleList::const_iterator triangleIterator = (*sceneObjectIterator)->decimalTriangleList()->begin();
                     triangleIterator != (*sceneObjectIterator)->decimalTriangleList()->end(); ++triangleIterator)
                {
                    for (int v = 0; v < 3; ++v)
                    {
                        updateMaximumFractionDigits(maximumNumberOfFractionDigits, triangleIterator->vertex(v).x());
                        updateMaximumFractionDigits(maximumNumberOfFractionDigits, triangleIterator->vertex(v).y());
                        updateMaximumFractionDigits(maximumNumberOfFractionDigits, triangleIterator->vertex(v).z());
                    }
                }
            }
        }
        break;
    }

    // generate scele factor
    if (maximumNumberOfFractionDigits > MAXIMUM_EXACT_TRUNCATION_DIGITS)
    {
        QMessageBox::warning(this,
                             tr("Floating-point scene"),
                             tr("Some of the coordinates have more than 32 significant fraction digits!\nTruncation is going to occur!"), QMessageBox::Ok);

        maximumNumberOfFractionDigits = MAXIMUM_EXACT_TRUNCATION_DIGITS;
    }

    QDecimal scaleFactor(1);

    for (int j = 0 ; j < maximumNumberOfFractionDigits; ++j)
        scaleFactor *= 10;

    // create scene
    switch (type)
    {
    case SceneObject::Type_DecimalBallList:
        {
            Ball_list_3_Z movable;
            Ball_list_3_Z obstacle;
            Ball_list_3_Z *target;

            // accumulate scene and rotating object
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin(); sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                if ((*sceneObjectIterator)->isRotating())
                    target = &movable;
                else
                    target = &obstacle;

                for (DecimalBallList::const_iterator ballIterator = (*sceneObjectIterator)->decimalBallList()->begin();
                     ballIterator != (*sceneObjectIterator)->decimalBallList()->end(); ++ballIterator)
                {
                    Z x = decimalToZ(scaleFactor * ballIterator->center().x());
                    Z y = decimalToZ(scaleFactor * ballIterator->center().y());
                    Z z = decimalToZ(scaleFactor * ballIterator->center().z());
                    Z r = decimalToZ(scaleFactor * ballIterator->radius());

                    target->push_back(Ball_3_Z(Vector_3_Z(x, y, z), r));
                }
            }

            if (movable.empty() || obstacle.empty())
            {
                QMessageBox::warning(this, tr("Invalid scene"), tr("Neither movable nor obstacles can be empty!"), QMessageBox::Ok);
                return;
            }

            // create raster
            exactConfigurationSpace.reset(
                new ExactConfigurationSpace(
                    ExactConfigurationSpaceTag<Spin_configuration_space_3::Exact_BB_Z>(),
                    movable.begin(), movable.end(),
                    obstacle.begin(), obstacle.end(),
                    Spin_configuration_space_3::Exact_BB_Z::Parameters(suppressQsicCalculation, suppressQsipCalculation),
                    suppressQuadricMeshing,
                    suppressQsicMeshing,
                    suppressQsipMeshing,
                    optionViewClipPlane,
                    m_widgetConfigurationView));
        }
        break;

    case SceneObject::Type_DecimalTriangleList:
        {
            Triangle_list_3_Z movable;
            Triangle_list_3_Z obstacle;
            Triangle_list_3_Z *target;

            // accumulate scene and rotating object
            for (SceneObjects::const_iterator sceneObjectIterator = m_sceneObjects.begin(); sceneObjectIterator != m_sceneObjects.end(); ++sceneObjectIterator)
            {
                if ((*sceneObjectIterator)->isRotating())
                    target = &movable;
                else
                    target = &obstacle;

                for (DecimalTriangleList::const_iterator triangleIterator = (*sceneObjectIterator)->decimalTriangleList()->begin();
                     triangleIterator != (*sceneObjectIterator)->decimalTriangleList()->end(); ++triangleIterator)
                {
                    Z x0 = decimalToZ(scaleFactor * triangleIterator->vertex(0).x());
                    Z y0 = decimalToZ(scaleFactor * triangleIterator->vertex(0).y());
                    Z z0 = decimalToZ(scaleFactor * triangleIterator->vertex(0).z());
                    Z x1 = decimalToZ(scaleFactor * triangleIterator->vertex(1).x());
                    Z y1 = decimalToZ(scaleFactor * triangleIterator->vertex(1).y());
                    Z z1 = decimalToZ(scaleFactor * triangleIterator->vertex(1).z());
                    Z x2 = decimalToZ(scaleFactor * triangleIterator->vertex(2).x());
                    Z y2 = decimalToZ(scaleFactor * triangleIterator->vertex(2).y());
                    Z z2 = decimalToZ(scaleFactor * triangleIterator->vertex(2).z());

                    target->push_back(Triangle_3_Z(Point_3_Z(x0, y0, z0),
                                                   Point_3_Z(x1, y1, z1),
                                                   Point_3_Z(x2, y2, z2)));
                }
            }

            if (movable.empty() || obstacle.empty())
            {
                QMessageBox::warning(this, tr("Invalid scene"), tr("Neither movable nor obstacles can be empty!"), QMessageBox::Ok);
                return;
            }

            // create raster
            exactConfigurationSpace.reset(
                new ExactConfigurationSpace(
                    ExactConfigurationSpaceTag<Spin_configuration_space_3::Exact_TT_Z>(),
                    movable.begin(), movable.end(),
                    obstacle.begin(), obstacle.end(),
                    Spin_configuration_space_3::Exact_TT_Z::Parameters(suppressQsicCalculation, suppressQsipCalculation),
                    suppressQuadricMeshing,
                    suppressQsicMeshing,
                    suppressQsipMeshing,
                    optionViewClipPlane,
                    m_widgetConfigurationView));
        }
        break;
    }

    // add configuration space to view
    ConfigurationObjectPtr configurationObject(new ConfigurationObject(exactConfigurationSpace));
    addConfigurationObject(configurationObject, "<generated>", QIcon(":/resource/img/recalculate.png"));
}

void ClientForm::on_tableWidgetSceneObjects_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);

    // get scene object
    SceneObjectPtr sceneObject = m_sceneObjects[row];

    // show edit window for scene object
    SceneObjectDialog dialog(
                sceneObject->typeString(),
                sceneObject->isVisible(),
                sceneObject->isRotating(),
                sceneObject->color(),
                "unavailable",
                this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // copy new settings
    sceneObject->setVisible(dialog.isObjectVisible());
    sceneObject->setRotating(dialog.isObjectRotating());
    sceneObject->setColor(dialog.objectColor());

    switch (sceneObject->type())
    {
    case SceneObject::Type_DecimalBallList:
        m_widgetSceneView->setDecimalBallListColor(sceneObject->decimalBallList(), dialog.objectColor());
        m_widgetSceneView->setDecimalBallListVisible(sceneObject->decimalBallList(), dialog.isObjectVisible());
        break;

    case SceneObject::Type_DecimalTriangleList:
        m_widgetSceneView->setDecimalTriangleListColor(sceneObject->decimalTriangleList(), dialog.objectColor());
        m_widgetSceneView->setDecimalTriangleListVisible(sceneObject->decimalTriangleList(), dialog.isObjectVisible());
        break;
    }

    // update table
    ui->tableWidgetSceneObjects->item(row, 1)->setText(boolToString(sceneObject->isVisible()));
    ui->tableWidgetSceneObjects->item(row, 2)->setText(boolToString(sceneObject->isRotating()));

    ui->tableWidgetSceneObjects->item(row, 0)->setBackgroundColor(sceneObject->color());

    // update rotations
    updateSceneObjectsRotations();
}

void ClientForm::on_labelConfigurationObjectDelete_linkActivated(const QString &link)
{
    Q_UNUSED(link);

    QList<QTableWidgetItem *> indexes = ui->tableWidgetConfigurationObjects->selectedItems();

    if (indexes.size() != ui->tableWidgetConfigurationObjects->columnCount())
        return;

    int row = indexes[0]->row();

    // remove it
    removeConfigurationObject(row);
}

void ClientForm::on_labelConfigurationObjectClear_linkActivated(const QString &link)
{
    Q_UNUSED(link);

    // TODO: optimize
    while (!m_configurationObjects.empty())
        removeConfigurationObject(0);
}

void ClientForm::on_toolButtonAddPredicate_clicked()
{
    VariantPredicateDialog dialog(VariantPredicate::Halfspace, this);
    dialog.exec();
}

void ClientForm::on_checkBoxSceneCullFaces_toggled(bool checked)
{
    m_widgetSceneView->setCullingEnabled(checked);
}

void ClientForm::on_checkBoxConfigurationCullFaces_toggled(bool checked)
{
    m_widgetConfigurationView->setCullingEnabled(checked);
}

void ClientForm::on_toolButtonOpenSceneTxt_clicked()
{
    // import sphere tree
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            tr("Open triangulated mesh"),
            QString(),
            tr("Triangulated mesh files (*.txt)"));

    if (fileName.isEmpty())
        return;

    SceneObjectPtr object = SceneObject::loadFromText(fileName.toStdString().c_str(), this);

    if (!object)
        return (void)QMessageBox::warning(this, tr("Open file"), tr("Failed to load file!"), QMessageBox::Ok);

    // register
    object->setColor(m_widgetSceneView->nextSuggestedColor());
    addSceneObject(object, fileName.toStdString().c_str());
}

void ClientForm::on_checkBoxSceneRenderingWireframe_toggled(bool checked)
{
    m_widgetSceneView->setWireframe(checked);
}

void ClientForm::on_checkBoxConfigurationRenderingWireframe_toggled(bool checked)
{
    m_widgetConfigurationView->setWireframe(checked);
}

void ClientForm::clearLogMessages()
{
    ui->textEditConfigurationConsole->clear();
}

void ClientForm::logMessage(QString loggerName, QString level, long long deltaTimeMicroSec, QString message)
{
    ui->textEditConfigurationConsole->append(
                QString("[") +
                QString::number(deltaTimeMicroSec / 1000000) +
                QString(".") +
                QString::number(deltaTimeMicroSec % 1000000) +
                QString("] ") +
                level +
                QString("  ") +
                loggerName +
                QString(": ") +
                message);
}

void ClientForm::on_labelConfigurationConsoleClear_linkActivated(const QString &link)
{
    Q_UNUSED(link);
    ui->textEditConfigurationConsole->clear();
}

void ClientForm::on_tableWidgetConfigurationObjects_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);

    // get scene object
    ConfigurationObjectPtr configurationObject = m_configurationObjects[row];

    // show edit window for scene object
    ConfigurationObjectDialog dialog(
                configurationObject->typeString(),
                configurationObject->isVisible(),
                "unavailable",
                this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // copy new settings
    configurationObject->setVisible(dialog.isObjectVisible());

/*
    switch (sceneObject->type())
    {
    case SceneObject::Type_BallList:
        m_widgetSceneView->setBallListVisible(sceneObject->ballList(), dialog.isObjectVisible());
        break;

    case SceneObject::Type_TriangleList:
        m_widgetSceneView->setTriangleListVisible(sceneObject->triangleList(), dialog.isObjectVisible());
        break;
    }
*/

    // update table
    ui->tableWidgetConfigurationObjects->item(row, 1)->setText(boolToString(configurationObject->isVisible()));
}

void ClientForm::on_tableWidgetConfigurationObjects_customContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem *item = ui->tableWidgetConfigurationObjects->itemAt(pos);

    if (!item)
        return;

    // display configuration space menu only for configuration spaces
    ConfigurationObjectPtr configurationObject = m_configurationObjects[item->row()];

    switch (configurationObject->type())
    {
    case ConfigurationObject::Type_RasterConfigurationSpace:
    case ConfigurationObject::Type_CellConfigurationSpace:
    case ConfigurationObject::Type_ExactConfigurationSpace: break;
    default: return;
    }

    QMenu *contextMenu = new QMenu(this);

    QAction *actionSaveConfigurationObject = new QAction(QIcon(":/resource/img/save.png"), tr("Save as..."), contextMenu);
    connect(actionSaveConfigurationObject, SIGNAL(triggered()), this, SLOT(saveConfigurationObjectTriggered()));
    contextMenu->addAction(actionSaveConfigurationObject);

    QAction *actionRouteConfigurationObject = new QAction(QIcon(":/resource/img/road.png"), tr("Find route"), contextMenu);
    connect(actionRouteConfigurationObject, SIGNAL(triggered()), this, SLOT(routeConfigurationObjectTriggered()));
    contextMenu->addAction(actionRouteConfigurationObject);

    int row = item->row();
    m_configurationObjectPopupRow = row;

    contextMenu->exec(QCursor::pos());
}

void ClientForm::saveConfigurationObjectTriggered()
{
    ConfigurationObjectPtr configurationObject = m_configurationObjects[m_configurationObjectPopupRow];

    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            tr("Save configuration object"),
            QString(),
            tr("Configuration space files (*.csp)"));

    if (fileName.isEmpty())
        return;

    QFileInfo file(fileName);

    if (file.suffix().isEmpty())
        fileName += ".csp";

    // execute save procedure
    configurationObject->saveToFile(fileName, this);
}

void ClientForm::routeConfigurationObjectTriggered()
{
    QQuaternion begin = motionBeginRotation();
    QQuaternion end = motionEndRotation();

    ConfigurationObjectPtr configurationObject = m_configurationObjects[m_configurationObjectPopupRow];
    RoutePtr route = configurationObject->findRoute(begin, end);

    if (!route)
        QMessageBox::warning(this, tr("Find route"), tr("Route not found"), QMessageBox::Ok);
    else
        QMessageBox::information(this, tr("Find route"), tr("Found a route"), QMessageBox::Ok);

    // if a route was found add it to configuration view
    if (route)
    {
        // add configuration object
        SampledRoutePtr sampledRoute(new SampledRoute(route, m_widgetConfigurationView));
        ConfigurationObjectPtr configurationObject(new ConfigurationObject(route, sampledRoute));
        addConfigurationObject(configurationObject, QString("<generated>"), QIcon(":/resource/img/road.png"));
    }
}

void ClientForm::on_toolButtonOpenConfigurationCSP_clicked()
{
    // import sphere tree
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            tr("Open configuration object"),
            QString(),
            tr("Configuration space files (*.csp)"));

    if (fileName.isEmpty())
        return;

    ConfigurationObjectPtr configurationObject = ConfigurationObject::loadFromFile(fileName, this, m_widgetConfigurationView);

    if (configurationObject)
        addConfigurationObject(configurationObject, fileName, QIcon(":/resource/img/recalculate.png"), QString(" <pre-rendered>"));
}

void ClientForm::toggleSceneFullScreenTriggered()
{
    ui->widgetGroupConfiguration->setVisible(!ui->widgetGroupConfiguration->isVisible());
    emit toggleFullScreen();
}

void ClientForm::toggleConfigurationFullScreenTriggered()
{
    ui->widgetGroupScene->setVisible(!ui->widgetGroupScene->isVisible());
    emit toggleFullScreen();
}

void ClientForm::updateAutomaticMotion()
{
    int value = ui->horizontalSliderMotion->value();
    value += 10;

    if (value >= ui->horizontalSliderMotion->maximum() + 1)
        value = ui->horizontalSliderMotion->minimum();

    ui->horizontalSliderMotion->setValue(value);
    emit on_horizontalSliderMotion_valueChanged(value);
}

void ClientForm::on_doubleSpinBoxMotionBeginYaw_valueChanged(double arg)
{
    Q_UNUSED(arg);
    updateSceneObjectsRotations();
}

void ClientForm::on_doubleSpinBoxMotionBeginPitch_valueChanged(double arg)
{
    Q_UNUSED(arg);
    updateSceneObjectsRotations();
}

void ClientForm::on_doubleSpinBoxMotionBeginRoll_valueChanged(double arg)
{
    Q_UNUSED(arg);
    updateSceneObjectsRotations();
}

void ClientForm::on_doubleSpinBoxMotionEndYaw_valueChanged(double arg)
{
    Q_UNUSED(arg);
    updateSceneObjectsRotations();
}

void ClientForm::on_doubleSpinBoxMotionEndPitch_valueChanged(double arg)
{
    Q_UNUSED(arg);
    updateSceneObjectsRotations();
}

void ClientForm::on_doubleSpinBoxMotionEndRoll_valueChanged(double arg)
{
    Q_UNUSED(arg);
    updateSceneObjectsRotations();
}

void ClientForm::on_pushButtonStartStopMotion_clicked()
{
    if (!checkRouteIsSelected())
        return;

    if (ui->pushButtonStartStopMotion->isChecked())
    {
        m_motionTimer = new QTimer(this);
        connect(m_motionTimer, SIGNAL(timeout()), this, SLOT(updateAutomaticMotion()));
        m_motionTimer->setSingleShot(false);
        m_motionTimer->start(100);
    }
    else
    {
        m_motionTimer->stop();
        delete m_motionTimer;
        m_motionTimer = 0;
    }
}

void ClientForm::on_horizontalSliderMotion_valueChanged(int value)
{
    ui->labelSliderMotionValue->setText(QString::number(value));

    if (!checkRouteIsSelected())
        return;

    // manual animation setup
    updateSceneObjectsRotations();
}

bool ClientForm::checkRouteIsSelected()
{
    if (!selectedRoute())
    {
        QMessageBox::warning(this, tr("No route was selected"), tr("Please select and/or compute a route from a configuration space"));
        return false;
    }

    return true;
}

void ClientForm::updateSceneObjectsRotations()
{
    MotionMode mode = motionMode();

    QQuaternion currentRotation;

    switch (mode)
    {
        case MotionMode_Begin:  currentRotation = motionBeginRotation(); break;
        case MotionMode_End:    currentRotation = motionEndRotation(); break;
        case MotionMode_Motion: currentRotation = motionRouteRotation(); break;
    }

    for (SceneObjects::iterator iterator = m_sceneObjects.begin(); iterator != m_sceneObjects.end(); ++iterator)
    {
        SceneObjectPtr sceneObject = *iterator;
        QQuaternion objectRotation;

        if (sceneObject->isRotating())
            objectRotation = currentRotation;

        switch (sceneObject->type())
        {
        case SceneObject::Type_DecimalTriangleList:
            m_widgetSceneView->setDecimalTriangleListQuaternion(sceneObject->decimalTriangleList(), objectRotation);
            break;

        case SceneObject::Type_DecimalBallList:
            m_widgetSceneView->setDecimalBallListQuaternion(sceneObject->decimalBallList(), objectRotation);
            break;
        }
    }
}

void ClientForm::on_pushButtonMotionModeBegin_clicked()
{
    selectMotionMode(MotionMode_Begin);
}

void ClientForm::on_pushButtonMotionModeMotion_clicked()
{
    selectMotionMode(MotionMode_Motion);
}

void ClientForm::on_pushButtonMotionModeEnd_clicked()
{
    selectMotionMode(MotionMode_End);
}

void ClientForm::selectMotionMode(MotionMode motionMode)
{
    switch (motionMode)
    {
    case MotionMode_Begin:
        ui->pushButtonMotionModeBegin->setChecked(true);
        ui->pushButtonMotionModeEnd->setChecked(false);
        ui->pushButtonMotionModeMotion->setChecked(false);
        break;

    case MotionMode_End:
        ui->pushButtonMotionModeBegin->setChecked(false);
        ui->pushButtonMotionModeEnd->setChecked(true);
        ui->pushButtonMotionModeMotion->setChecked(false);
        break;

    case MotionMode_Motion:
        ui->pushButtonMotionModeBegin->setChecked(false);
        ui->pushButtonMotionModeEnd->setChecked(false);
        ui->pushButtonMotionModeMotion->setChecked(true);
        break;
    }

    updateSceneObjectsRotations();
}

ClientForm::MotionMode ClientForm::motionMode() const
{
    if (ui->pushButtonMotionModeBegin->isChecked())
        return MotionMode_Begin;

    if (ui->pushButtonMotionModeEnd->isChecked())
        return MotionMode_End;

    if (ui->pushButtonMotionModeMotion->isChecked())
        return MotionMode_Motion;

    assert(0);
    return MotionMode_Motion;
}

QQuaternion ClientForm::motionBeginRotation() const
{
    return quaternionFromEulerAngles(ui->doubleSpinBoxMotionBeginRoll->value(),
                                     ui->doubleSpinBoxMotionBeginPitch->value(),
                                     ui->doubleSpinBoxMotionBeginYaw->value());
}

QQuaternion ClientForm::motionEndRotation() const
{
    return quaternionFromEulerAngles(ui->doubleSpinBoxMotionEndRoll->value(),
                                     ui->doubleSpinBoxMotionEndPitch->value(),
                                     ui->doubleSpinBoxMotionEndYaw->value());
}

QQuaternion ClientForm::motionRouteRotation() const
{
    RoutePtr route = selectedRoute();

    // is there any route
    if (!route)
        return QQuaternion();

    // evaluate rotation from the route
    return route->evaluate(static_cast<double>(ui->horizontalSliderMotion->value()) / static_cast<double>(ui->horizontalSliderMotion->maximum()));
}

void ClientForm::on_toolButtonSaveSceneArr_clicked()
{
    // save the whole arrangement along with all settings
    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            tr("Save scene"),
            QString(),
            tr("Scene files (*.arr)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);

    if (fileInfo.suffix().isEmpty())
        fileName += ".arr";

    // save all scene objects
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly))
        return;

    QDataStream dataStream(&file);

    // scene objects
    dataStream << static_cast<int>(m_sceneObjects.size());

    for (SceneObjects::const_iterator it = m_sceneObjects.begin(); it != m_sceneObjects.end(); ++it)
        (*it)->saveToStream(dataStream);

    // motion
    dataStream << ui->doubleSpinBoxMotionBeginYaw->value();
    dataStream << ui->doubleSpinBoxMotionBeginPitch->value();
    dataStream << ui->doubleSpinBoxMotionBeginRoll->value();

    dataStream << ui->doubleSpinBoxMotionEndYaw->value();
    dataStream << ui->doubleSpinBoxMotionEndPitch->value();
    dataStream << ui->doubleSpinBoxMotionEndRoll->value();
}

void ClientForm::on_toolButtonOpenSceneArr_clicked()
{
    // open a whole arrangement along with all settings
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            tr("Open scene"),
            QString(),
            tr("Scene files (*.arr)"));

    if (fileName.isEmpty())
        return;

    // clear current scene
    while (!m_sceneObjects.empty())
        removeSceneObject(0);

    // read all scene objects
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
        return;

    QDataStream dataStream(&file);

    if (!readSceneFromStream(dataStream))
    {
        // clear partial read scene
        while (!m_sceneObjects.empty())
            removeSceneObject(0);

        return (void)QMessageBox::warning(this, tr("Open scene"), tr("Failed to load scene!"), QMessageBox::Ok);
    }
}

bool ClientForm::readSceneFromStream(QDataStream &dataStream)
{
    // scene objects
    int numberOfSceneObjects;

    dataStream >> numberOfSceneObjects;
    if (dataStream.status() != QDataStream::Ok) return false;

    for (int i = 0; i < numberOfSceneObjects; ++i)
    {
        SceneObjectPtr sceneObject = SceneObject::loadFromStream(dataStream);

        if (!sceneObject)
            return false;

        addSceneObject(sceneObject, QString("<loaded>"));
    }

    // motion
    double beginYaw, beginPitch, beginRoll, endYaw, endPitch, endRoll;

    dataStream >> beginYaw;
    if (dataStream.status() != QDataStream::Ok) return false;

    dataStream >> beginPitch;
    if (dataStream.status() != QDataStream::Ok) return false;

    dataStream >> beginRoll;
    if (dataStream.status() != QDataStream::Ok) return false;

    dataStream >> endYaw;
    if (dataStream.status() != QDataStream::Ok) return false;

    dataStream >> endPitch;
    if (dataStream.status() != QDataStream::Ok) return false;

    dataStream >> endRoll;
    if (dataStream.status() != QDataStream::Ok) return false;

    ui->doubleSpinBoxMotionBeginYaw->setValue(beginYaw);
    ui->doubleSpinBoxMotionBeginPitch->setValue(beginPitch);
    ui->doubleSpinBoxMotionBeginRoll->setValue(beginRoll);

    ui->doubleSpinBoxMotionEndYaw->setValue(endYaw);
    ui->doubleSpinBoxMotionEndPitch->setValue(endPitch);
    ui->doubleSpinBoxMotionEndRoll->setValue(endRoll);

    return true;
}

RoutePtr ClientForm::selectedRoute() const
{
    QList<QTableWidgetItem *> items = ui->tableWidgetConfigurationObjects->selectedItems();

    if (items.size() != ui->tableWidgetConfigurationObjects->columnCount())
        return RoutePtr();

    int row = items.at(0)->row();
    ConfigurationObjectPtr configurationObject = m_configurationObjects[row];
    return configurationObject->route();
}

void ClientForm::on_tableWidgetConfigurationObjects_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);

    // update current rotations (next frame because the item is not yet selected)
    QTimer::singleShot(0, this, SLOT(updateSceneObjectsRotations()));
}
