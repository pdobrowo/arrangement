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
#ifndef CLIENTFORM_H
#define CLIENTFORM_H

#include "sceneobject.h"
#include "configurationobject.h"
#include "configurationspace.h"
#include <QWidget>
#include <QQuaternion>
#include <QDataStream>
#include <boost/scoped_ptr.hpp>

namespace Ui {
class ClientForm;
}

class QTimer;
class QTableWidgetItem;
class RenderView;

class ClientForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit ClientForm(QWidget *parent = 0);
    ~ClientForm();
    
private:
    enum CameraType
    {
        CT_ArcBall,
        CT_Flying,
        CT_Automatic
    };

    void    setSceneCamera(CameraType type);
    void    setConfigurationCamera(CameraType type);

    size_t  selectRasterResolution();
    size_t  selectCellSampleCount();

    enum MotionMode
    {
        MotionMode_Begin,
        MotionMode_End,
        MotionMode_Motion
    };

    void        selectMotionMode(MotionMode motionMode);
    MotionMode  motionMode() const;
    QQuaternion motionBeginRotation() const;
    QQuaternion motionEndRotation() const;
    QQuaternion motionRouteRotation() const;

    // render windows / shared lists
    RenderView *m_widgetConfigurationView;
    RenderView *m_widgetSceneView;

private slots:
    void clearLogMessages();
    void logMessage(QString loggerName, QString level, long long deltaTimeMicroSec, QString message);

    void convertToRasterGraphTexture3DTriggered();
    void convertToRasterGraphGaussianSplatterTriggered();
    void convertToCellGraphGaussianSplatterTriggered();
    void convertToExactGraphTriggered();

    void saveConfigurationObjectTriggered();
    void routeConfigurationObjectTriggered();

    void toggleSceneFullScreenTriggered();
    void toggleConfigurationFullScreenTriggered();

    void updateAutomaticMotion();
    void updateSceneObjectsRotations();

private slots:
    void on_toolButtonSceneCameraArcBall_clicked();
    void on_toolButtonSceneCameraFlying_clicked();
    void on_toolButtonSceneCameraAutomatic_clicked();
    void on_toolButtonConfigurationCameraArcBall_clicked();
    void on_toolButtonConfigurationCameraFlying_clicked();
    void on_toolButtonConfigurationCameraAutomatic_clicked();
    void on_toolButtonOpenSceneSph_clicked();
    void on_toolButtonOpenSceneDir_clicked();
    void on_labelSceneObjectDelete_linkActivated(const QString &link);
    void on_labelSceneObjectClear_linkActivated(const QString &link);
    void on_tableWidgetSceneObjects_cellDoubleClicked(int row, int column);
    void on_labelConfigurationObjectDelete_linkActivated(const QString &link);
    void on_labelConfigurationObjectClear_linkActivated(const QString &link);
    void on_toolButtonAddPredicate_clicked();
    void on_checkBoxSceneCullFaces_toggled(bool checked);
    void on_checkBoxConfigurationCullFaces_toggled(bool checked);
    void on_toolButtonOpenSceneTxt_clicked();
    void on_checkBoxSceneRenderingWireframe_toggled(bool checked);
    void on_checkBoxConfigurationRenderingWireframe_toggled(bool checked);
    void on_labelConfigurationConsoleClear_linkActivated(const QString &link);
    void on_tableWidgetConfigurationObjects_cellDoubleClicked(int row, int column);
    void on_tableWidgetConfigurationObjects_customContextMenuRequested(const QPoint &pos);
    void on_toolButtonOpenConfigurationCSP_clicked();
    void on_doubleSpinBoxMotionEndYaw_valueChanged(double arg);
    void on_doubleSpinBoxMotionBeginYaw_valueChanged(double arg);
    void on_doubleSpinBoxMotionBeginPitch_valueChanged(double arg);
    void on_doubleSpinBoxMotionBeginRoll_valueChanged(double arg);
    void on_doubleSpinBoxMotionEndPitch_valueChanged(double arg);
    void on_doubleSpinBoxMotionEndRoll_valueChanged(double arg);
    void on_pushButtonStartStopMotion_clicked();
    void on_horizontalSliderMotion_valueChanged(int value);
    void on_pushButtonMotionModeBegin_clicked();
    void on_pushButtonMotionModeMotion_clicked();
    void on_pushButtonMotionModeEnd_clicked();
    void on_toolButtonSaveSceneArr_clicked();
    void on_toolButtonOpenSceneArr_clicked();
    void on_tableWidgetConfigurationObjects_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

private:
    // scene related
    typedef std::vector<SceneObjectPtr> SceneObjects;
    SceneObjects            m_sceneObjects;

    void                    addSceneObject(SceneObjectPtr object, const QString &fileName);
    void                    removeSceneObject(int row);

    // configuration space related
    typedef std::vector<ConfigurationObjectPtr> ConfigurationObjects;
    ConfigurationObjects    m_configurationObjects;

    void                    addConfigurationObject(ConfigurationObjectPtr object, const QString &fileName, const QIcon &icon, const QString &extraInfo = QString());
    void                    removeConfigurationObject(int row);

    bool                    checkSceneObjects(SceneObject::Type *outType);

    void                    createRasterConfigurationSpace(SceneObject::Type type, VolumeRendererType volumeRendererType);
    void                    createCellConfigurationSpace(SceneObject::Type type);
    void                    createExactConfigurationSpace(SceneObject::Type type);

    int                     m_configurationObjectPopupRow;

    // other
    bool                    readSceneFromStream(QDataStream &dataStream);

    // route
    RoutePtr                selectedRoute() const;
    QTimer *                m_motionTimer;

    bool                    checkRouteIsSelected();

signals:
    void                    toggleFullScreen();

private:
    Ui::ClientForm *ui;
};

#endif // CLIENTFORM_H
