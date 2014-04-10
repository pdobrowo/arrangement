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
#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include "renderviewcamera.h"
#include "decimalscene.h"
#include "shader.h"
#include "gridmesh.h"
#include "trianglelistmesh.h"
#include "polyconemesh.h"
#include "ballmesh.h"
#include "configurationspace.h"
#include <QVector3D>
#include <QQuaternion>
#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QString>
#include <QPainter>
#include <QLabel>
#include <QPicture>
#include <QImage>
#include <QGLWidget>
#include <boost/scoped_ptr.hpp>
#include <libqi.h>
#include <variantpredicate.h>
#include <map>
#include <set>

class QPaintEvent;
class QMouseEvent;

class MeshData
{
public:
    MeshData();
    virtual ~MeshData();

    void        setVisible(bool visible);
    bool        isVisible() const;

    void        setTranslation(const QVector3D &translation);
    QVector3D   translation() const;

    void        setQuaternion(const QQuaternion &quaternion);
    QQuaternion quaternion() const;

protected:
    bool        m_visible;
    QVector3D   m_translation;
    QQuaternion m_quaternion;
};

class TriangleListData
    : public MeshData
{
public:
    TriangleListData(QGLWidget *gl,
                     DecimalTriangleListPtr triangleList,
                     QColor color);

    void    render(bool wireframe);

    void    setColor(QColor color);
    QColor  color() const;

private:
    TriangleListMesh    m_renderObject;
    QColor              m_color;
};

typedef boost::shared_ptr<TriangleListData> TriangleListDataPtr;
typedef std::map<DecimalTriangleListPtr, TriangleListDataPtr> TriangleLists;

class SpinListData
    : public MeshData
{
public:
    SpinListData(QGLWidget *gl,
                 Qsic_spin_list_3_Z_ptr spinList);

    void render();

private:
    PolyConeMesh    m_renderObject;
};

typedef boost::shared_ptr<SpinListData> SpinListDataPtr;
typedef std::map<Qsic_spin_list_3_Z_ptr, SpinListDataPtr> SpinLists;

typedef std::map<Qsip_spin_3_Z_ptr, bool> Spins;

class BallListData
    : public MeshData
{
public:
    BallListData(DecimalBallListPtr ballList, QColor color);

    void    render(BallMesh *ball);

    void    setColor(QColor color);
    QColor  color() const;

private:
    DecimalBallListPtr  m_ballList;
    QColor              m_color;
};

typedef boost::shared_ptr<BallListData> BallListDataPtr;
typedef std::map<DecimalBallListPtr, BallListDataPtr> BallLists;

typedef std::set<ConfigurationSpacePtr> ConfigurationSpaces;

class RenderView
    : public QGLWidget
{
    Q_OBJECT

private:
    // view title
    QString m_caption;

    // view colors
    QColor m_textColor;

    // view font
    QFont m_textFont;

    // camera
    boost::scoped_ptr<RenderViewCamera> m_camera;

    // meshes
    boost::scoped_ptr<GridMesh> m_grid;
    boost::scoped_ptr<BallMesh> m_qsipBall;
    boost::scoped_ptr<BallMesh> m_unitBall;

    // axies
    void                        drawAxies();

    // gradient background
    void                        drawGradientBackground();

    // render objects
    TriangleLists               m_triangleLists;
    SpinLists                   m_spinLists;
    Spins                       m_spins;
    BallLists                   m_ballLists;
    ConfigurationSpaces         m_configurationSpaces;

    size_t                      m_nextSuggestedColor;

    // meshes
    void                        renderMeshes();
    void                        renderQsip(const Qsip_spin_3_Z_ptr qsip, bool visible);

    // view options
    bool                        m_wireframe;
    bool                        m_cullingEnabled;

    boost::scoped_ptr<Shader>   m_perPixelLightingShader;

    void                        ctor();

protected:
    virtual void                mouseMoveEvent(QMouseEvent *event);
    virtual void                mousePressEvent(QMouseEvent *event);
    virtual void                mouseReleaseEvent(QMouseEvent *event);
    virtual void                wheelEvent(QWheelEvent *event);
    virtual void                keyPressEvent(QKeyEvent *event);

    virtual void                mouseDoubleClickEvent(QMouseEvent *event);

    virtual void                initializeGL();
    virtual void                resizeGL(int w, int h);
    virtual void                paintGL();

public:
    explicit                    RenderView(QWidget* parent = 0, const QGLWidget *shareWidget = 0, Qt::WindowFlags f = 0);
    explicit                    RenderView(QGLContext *context, QWidget *parent = 0, const QGLWidget *shareWidget = 0, Qt::WindowFlags f = 0);
    explicit                    RenderView(const QGLFormat &format, QWidget *parent = 0, const QGLWidget *shareWidget = 0, Qt::WindowFlags f = 0);
    virtual                     ~RenderView();

    void                        setCaption(const QString& value);
    QString                     caption() const;

    void                        setTextFont(const QFont& value);
    QFont                       textFont() const;

    QColor                      nextSuggestedColor();

    // triangle lists
    void                        addDecimalTriangleList(DecimalTriangleListPtr triangleList, QColor color);
    void                        removeDecimalTriangleList(DecimalTriangleListPtr triangleList);

    void                        setDecimalTriangleListVisible(DecimalTriangleListPtr triangleList, bool visible);
    bool                        isDecimalTriangleListVisible(DecimalTriangleListPtr triangleList);

    void                        setDecimalTriangleListTranslation(DecimalTriangleListPtr triangleList, const QVector3D &translation);
    QVector3D                   decimalTriangleListTranslation(DecimalTriangleListPtr triangleList);

    void                        setDecimalTriangleListQuaternion(DecimalTriangleListPtr triangleList, const QQuaternion &rotation);
    QQuaternion                 decimalTriangleListQuaternion(DecimalTriangleListPtr triangleList);

    void                        setDecimalTriangleListColor(DecimalTriangleListPtr triangleList, const QColor &color);
    QColor                      decimalTriangleListColor(DecimalTriangleListPtr triangleList);

    // spin qsic
    void                        addSpinList(Qsic_spin_list_3_Z_ptr spinList);
    void                        removeSpinList(Qsic_spin_list_3_Z_ptr spinList);

    void                        setSpinListVisible(Qsic_spin_list_3_Z_ptr spinList, bool visible);
    bool                        isSpinListVisible(Qsic_spin_list_3_Z_ptr spinList);

    // spin qsip
    void                        addSpin(Qsip_spin_3_Z_ptr spin);
    void                        removeSpin(Qsip_spin_3_Z_ptr spin);

    void                        setSpinVisible(Qsip_spin_3_Z_ptr spin, bool visible);
    bool                        isSpinVisible(Qsip_spin_3_Z_ptr spin);

    // ball lists
    void                        addDecimalBallList(DecimalBallListPtr ballList, QColor color);
    void                        removeDecimalBallList(DecimalBallListPtr ballList);

    void                        setDecimalBallListVisible(DecimalBallListPtr ballList, bool visible);
    bool                        isDecimalBallListVisible(DecimalBallListPtr triangleList);

    void                        setDecimalBallListTranslation(DecimalBallListPtr triangleList, const QVector3D &translation);
    QVector3D                   decimalBallListTranslation(DecimalBallListPtr triangleList);

    void                        setDecimalBallListQuaternion(DecimalBallListPtr triangleList, const QQuaternion &rotation);
    QQuaternion                 decimalBallListQuaternion(DecimalBallListPtr triangleList);

    void                        setDecimalBallListColor(DecimalBallListPtr ballList, const QColor &color);
    QColor                      decimalBallListColor(DecimalBallListPtr ballList);

    // configuration spaces
    void                        addConfigurationSpace(ConfigurationSpacePtr configurationSpace);
    void                        removeConfigurationSpace(ConfigurationSpacePtr configurationSpace);

    // cleanup
    void                        removeAllObjects();

    // camera
    void                        setRenderViewCamera(RenderViewCamera *camera);

    // other
    bool                        isCullingEnabled() const;
    void                        setCullingEnabled(bool enabled);

    bool                        isWireframe() const;
    void                        setWireframe(bool wireframe);

signals:
    void                        toggleFullScreenTriggered();
};

#endif // RENDERVIEW_H
