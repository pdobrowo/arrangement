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
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "renderview.h"
#include "renderviewflycamera.h"
#include "renderviewarcballcamera.h"
#include <cs/Spin_qsic_mesh_3.h>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QVector>
#include <QApplication>
#include <stdlib.h>
#include <cmath>

namespace // anonymous
{
static QColor OBJECT_COLOR[] =
{
    QColor(Qt::red),
    QColor(Qt::green),
    QColor(Qt::blue),
    QColor(Qt::cyan),
    QColor(Qt::magenta),
    QColor(Qt::gray)
};

static const size_t NUM_OBJECT_COLORS = sizeof(OBJECT_COLOR) / sizeof(OBJECT_COLOR[0]);

// QSIC
const double    QSIC_RADIUS = 0.008;
const int       QSIC_SIDES  = 24;

// QSIP
const double    QSIP_RADIUS = 0.02;
const int       QSIP_SLICES = 24;
const int       QSIP_STACKS = 24;

void applyRotationGL(const QQuaternion &quaternion)
{
    double x2 = quaternion.x() * quaternion.x();
    double y2 = quaternion.y() * quaternion.y();
    double z2 = quaternion.z() * quaternion.z();
    double xy = quaternion.x() * quaternion.y();
    double xz = quaternion.x() * quaternion.z();
    double yz = quaternion.y() * quaternion.z();
    double wx = quaternion.scalar() * quaternion.x();
    double wy = quaternion.scalar() * quaternion.y();
    double wz = quaternion.scalar() * quaternion.z();

    double matrix[16] =
    {
        1.0 - 2.0 * (y2 + z2),  2.0 * (xy - wz),        2.0 * (xz + wy),        0.0,
        2.0 * (xy + wz),        1.0 - 2.0 * (x2 + z2),  2.0 * (yz - wx),        0.0,
        2.0 * (xz - wy),        2.0 * (yz + wx),        1.0 - 2.0 * (x2 + y2),  0.0,
        0.0,                    0.0,                    0.0,                    1.0
    };

    glMultMatrixd(matrix);
}
} // namespace anonymous

RenderView::RenderView(QWidget *parent, const RenderView::QGLWidget *shareWidget, Qt::WindowFlags f)
    : QGLWidget(parent, shareWidget, f)
{
    ctor();
}

RenderView::RenderView(QGLContext *context, QWidget *parent, const RenderView::QGLWidget *shareWidget, Qt::WindowFlags f)
    : QGLWidget(context, parent, shareWidget, f)
{
    ctor();
}

RenderView::RenderView(const QGLFormat &format, QWidget *parent, const RenderView::QGLWidget *shareWidget, Qt::WindowFlags f)
    : QGLWidget(format, parent, shareWidget, f)
{
    ctor();
}

void RenderView::ctor()
{
    m_nextSuggestedColor = 0;
    m_wireframe = false;
    m_cullingEnabled = false;

    setMouseTracking(true);

    // Default values
    m_caption = QString("unknown");
    m_textColor = QColor(255, 255, 128);
    m_textFont = QFont(QApplication::font().family(), 10);

    // Default camera
    m_camera.reset(new RenderViewArcBallCamera());
}

RenderView::~RenderView()
{
}

void RenderView::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    m_camera->mouseButtonPressed(rect(), event->button(), event->pos());
    updateGL();
}

void RenderView::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    m_camera->mouseMoved(rect(), event->pos());
    updateGL();
}

void RenderView::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    m_camera->mouseButtonReleased(rect(), event->button(), event->pos());
    updateGL();
}

void RenderView::wheelEvent(QWheelEvent *event)
{
    QGLWidget::wheelEvent(event);

    // Handle mouse wheel
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    m_camera->mouseWheelMoved(rect(), numSteps);
    updateGL();
}

QString RenderView::caption() const
{
    return m_caption;
}

void RenderView::setCaption(const QString& value)
{
    m_caption = value;
}

QFont RenderView::textFont() const
{
    return m_textFont;
}

QColor RenderView::nextSuggestedColor()
{
    return OBJECT_COLOR[(m_nextSuggestedColor++) % NUM_OBJECT_COLORS];
}

void RenderView::setTextFont(const QFont& value)
{
    m_textFont = value;

    // update meshes
    m_grid->setTextFont(m_textFont);
    m_grid->reset();
}

void RenderView::initializeGL()
{
    // init extensions
    glewInit();

    // init perpixel shader
    m_perPixelLightingShader.reset(
        new Shader("perpixel.vert", "perpixel.frag"));

    // Set perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0f, static_cast<GLfloat>(width()) / static_cast<GLfloat>(height()), 0.1f, 1024.0f);

    // Set viewport
    glViewport(0, 0, static_cast<GLsizei>(width()), static_cast<GLsizei>(height()));

    // Set modelview
    glMatrixMode(GL_MODELVIEW);

    // Enable culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Front face definition
    glFrontFace(GL_CCW);

    // Disable texturing
    glDisable(GL_TEXTURE_2D);

    // Smooth shading
    glShadeModel(GL_SMOOTH);

    // Clear mode
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    // Depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Somewhere in the initialization part of your program?
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Create light components
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // Nice perspective
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Multisampling
    if (0)
    {
        glEnable(GL_MULTISAMPLE);
        GLint bufs;
        GLint samples;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
        glGetIntegerv(GL_SAMPLES, &samples);
        qDebug("Multisampling: %i buffers and %i samples", bufs, samples);
    }

    // prerender meshes
    m_grid.reset(new GridMesh(this));
    m_grid->setTextColor(m_textColor);
    m_grid->setTextFont(m_textFont);

    m_qsipBall.reset(new BallMesh(this, QSIP_RADIUS, QSIP_SLICES, QSIP_STACKS));

    m_unitBall.reset(new BallMesh(this, 1.0, QSIP_SLICES, QSIP_STACKS));
}

void RenderView::resizeGL(int, int)
{
    // Set perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<GLfloat>(width()) / static_cast<GLfloat>(height()), 0.01f, 1024.0f);

    // Set viewport
    glViewport(0, 0, static_cast<GLsizei>(width()), static_cast<GLsizei>(height()));

    // Set modelview
    glMatrixMode(GL_MODELVIEW);
}

void RenderView::paintGL()
{
    // Clear view
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw gradient background
    drawGradientBackground();

    // Setup camera
    m_camera->applyTransformGL();

    // draw axies
    drawAxies();

    // culling enabled for conf-spaces, but not for scenes
    if (m_cullingEnabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    // draw contents
    renderMeshes();

    // Draw panel name
    qglColor(m_textColor);
    renderText(5, 15, m_caption, m_textFont);

    QString percent;
    percent.sprintf("%.0f%%", m_camera->zoom() * 100);
    renderText(width() - 45, 15, percent, m_textFont);
}

void RenderView::drawGradientBackground()
{
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Autodesk Maya 2011 background
    glBegin(GL_QUADS);
        glColor3ub(125, 145, 165);
        glVertex2i(-1, 1);
        glVertex2i( 1, 1);

        glColor3ub(26, 27, 29);
        glVertex2i( 1, -1);
        glVertex2i(-1, -1);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}

void RenderView::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit toggleFullScreenTriggered();
}

void RenderView::drawAxies()
{
    glDisable(GL_LIGHTING);
    m_grid->render();
}

void RenderView::addDecimalTriangleList(DecimalTriangleListPtr triangleList, QColor color)
{
    if (m_triangleLists.find(triangleList) != m_triangleLists.end())
        return;

    TriangleListDataPtr data(new TriangleListData(this, triangleList, color));
    m_triangleLists[triangleList] = data;

    updateGL();
}

void RenderView::renderMeshes()
{
    if (m_wireframe)
    {
        // wireframe rendering
        glDisable(GL_LIGHTING);
    }
    else
    {
        // normal rendering
        glEnable(GL_LIGHTING);
        m_perPixelLightingShader->bind();
    }

    // draw triangle soups
    for (TriangleLists::const_iterator it = m_triangleLists.begin(); it != m_triangleLists.end(); ++it)
        it->second->render(m_wireframe);

    if (m_wireframe)
    {
        glEnable(GL_LIGHTING);
        m_perPixelLightingShader->bind();
    }

    // draw spin qsics
    for (SpinLists::const_iterator it = m_spinLists.begin(); it != m_spinLists.end(); ++it)
        it->second->render();

    // draw spin qsips
    for (Spins::const_iterator it = m_spins.begin(); it != m_spins.end(); ++it)
        renderQsip(it->first, it->second);

    // draw balls
    for (BallLists::const_iterator it = m_ballLists.begin(); it != m_ballLists.end(); ++it)
        it->second->render(m_unitBall.get());

    m_perPixelLightingShader->unbind();
    glDisable(GL_LIGHTING);

    // draw configuration spaces
    for (ConfigurationSpaces::const_iterator it = m_configurationSpaces.begin(); it != m_configurationSpaces.end(); ++it)
    {
        // enable optional lighting
        if ((*it)->needsLighting())
        {
            glEnable(GL_LIGHTING);

            if ((*it)->needsShaderLighting())
                m_perPixelLightingShader->bind();
        }

        // render configuration space
        (*it)->render();

        // disable optional lighting
        if ((*it)->needsLighting())
        {
            if ((*it)->needsShaderLighting())
                m_perPixelLightingShader->unbind();

            glDisable(GL_LIGHTING);
        }
    }
}

void RenderView::setDecimalTriangleListVisible(DecimalTriangleListPtr triangleList, bool visible)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return;

    m_triangleLists[triangleList]->setVisible(visible);

    updateGL();
}

void RenderView::setDecimalBallListVisible(DecimalBallListPtr ballList, bool visible)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return;

    m_ballLists[ballList]->setVisible(visible);

    updateGL();
}

void RenderView::addSpinList(Qsic_spin_list_3_Z_ptr spinList)
{
    if (m_spinLists.find(spinList) != m_spinLists.end())
        return;

    SpinListDataPtr data(new SpinListData(this, spinList));
    m_spinLists[spinList] = data;

    updateGL();
}

void RenderView::addSpin(Qsip_spin_3_Z_ptr spin)
{
    m_spins[spin] = true;
    updateGL();
}

void RenderView::renderQsip(const Qsip_spin_3_Z_ptr spin, bool visible)
{
    // do not render unvisible qspips
    if (!visible)
        return;

    // material
    glColor3ub(255, 127, 0);

    // draw qsic
    glPushMatrix();
        glTranslated(spin->s12(), spin->s23(), spin->s31());
        m_qsipBall->render();
    glPopMatrix();
}

void RenderView::keyPressEvent(QKeyEvent *event)
{
    const double MOVE_AMOUNT = 4.0;

    switch (event->key())
    {
    case Qt::Key_Up:
    case Qt::Key_W:
        m_camera->moveForward(MOVE_AMOUNT);
        return;

    case Qt::Key_Down:
    case Qt::Key_S:
        m_camera->moveForward(-MOVE_AMOUNT);
        return;

    case Qt::Key_Right:
    case Qt::Key_D:
        m_camera->strafeRight(MOVE_AMOUNT);
        return;

    case Qt::Key_Left:
    case Qt::Key_A:
        m_camera->strafeRight(-MOVE_AMOUNT);
        return;
    }

    // call base member
    QGLWidget::keyPressEvent(event);
}

void RenderView::setRenderViewCamera(RenderViewCamera *camera)
{
    m_camera.reset(camera);
    updateGL();
}

bool RenderView::isCullingEnabled() const
{
    return m_cullingEnabled;
}

void RenderView::setCullingEnabled(bool enabled)
{
    m_cullingEnabled = enabled;
    updateGL();
}

bool RenderView::isWireframe() const
{
    return m_wireframe;
}

void RenderView::setWireframe(bool wireframe)
{
    m_wireframe = wireframe;
    updateGL();
}

void RenderView::removeDecimalTriangleList(DecimalTriangleListPtr triangleList)
{
    TriangleLists::iterator it = m_triangleLists.find(triangleList);

    if (it == m_triangleLists.end())
        return;

    m_triangleLists.erase(it);

    updateGL();
}

bool RenderView::isDecimalTriangleListVisible(DecimalTriangleListPtr triangleList)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return false;

    return m_triangleLists[triangleList]->isVisible();
}

bool RenderView::isDecimalBallListVisible(DecimalBallListPtr ballList)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return false;

    return m_ballLists[ballList]->isVisible();
}

void RenderView::setDecimalTriangleListTranslation(DecimalTriangleListPtr triangleList, const QVector3D &translation)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return;

    m_triangleLists[triangleList]->setTranslation(translation);

    updateGL();
}

QVector3D RenderView::decimalTriangleListTranslation(DecimalTriangleListPtr triangleList)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return QVector3D();

    return m_triangleLists[triangleList]->translation();
}

void RenderView::setDecimalTriangleListQuaternion(DecimalTriangleListPtr triangleList, const QQuaternion &quaternion)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return;

    m_triangleLists[triangleList]->setQuaternion(quaternion);

    updateGL();
}

QQuaternion RenderView::decimalTriangleListQuaternion(DecimalTriangleListPtr triangleList)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return QQuaternion();

    return m_triangleLists[triangleList]->quaternion();
}

void RenderView::setDecimalBallListTranslation(DecimalBallListPtr ballList, const QVector3D &translation)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return;

    m_ballLists[ballList]->setTranslation(translation);

    updateGL();
}

QVector3D RenderView::decimalBallListTranslation(DecimalBallListPtr ballList)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return QVector3D();

    return m_ballLists[ballList]->translation();
}

void RenderView::setDecimalBallListQuaternion(DecimalBallListPtr ballList, const QQuaternion &quaternion)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return;

    m_ballLists[ballList]->setQuaternion(quaternion);

    updateGL();
}

QQuaternion RenderView::decimalBallListQuaternion(DecimalBallListPtr ballList)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return QQuaternion();

    return m_ballLists[ballList]->quaternion();
}

void RenderView::setDecimalTriangleListColor(DecimalTriangleListPtr triangleList, const QColor &color)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return;

    m_triangleLists[triangleList]->setColor(color);

    updateGL();
}

QColor RenderView::decimalTriangleListColor(DecimalTriangleListPtr triangleList)
{
    if (m_triangleLists.find(triangleList) == m_triangleLists.end())
        return QColor();

    return m_triangleLists[triangleList]->color();
}

void RenderView::removeSpinList(Qsic_spin_list_3_Z_ptr spinList)
{
    SpinLists::iterator it = m_spinLists.find(spinList);

    if (it == m_spinLists.end())
        return;

    m_spinLists.erase(it);

    updateGL();
}

void RenderView::removeSpin(Qsip_spin_3_Z_ptr spin)
{
    Spins::iterator it = m_spins.find(spin);

    if (it == m_spins.end())
        return;

    m_spins.erase(it);

    updateGL();
}

void RenderView::setSpinListVisible(Qsic_spin_list_3_Z_ptr spinList, bool visible)
{
    if (m_spinLists.find(spinList) == m_spinLists.end())
        return;

    m_spinLists[spinList]->setVisible(visible);

    updateGL();
}

void RenderView::setSpinVisible(Qsip_spin_3_Z_ptr spin, bool visible)
{
    if (m_spins.find(spin) == m_spins.end())
        return;

    m_spins[spin] = visible;

    updateGL();
}

bool RenderView::isSpinListVisible(Qsic_spin_list_3_Z_ptr spinList)
{
    if (m_spinLists.find(spinList) == m_spinLists.end())
        return false;

    return m_spinLists[spinList]->isVisible();
}

bool RenderView::isSpinVisible(Qsip_spin_3_Z_ptr spin)
{
    if (m_spins.find(spin) == m_spins.end())
        return false;

    return m_spins[spin];
}

void RenderView::addDecimalBallList(DecimalBallListPtr ballList, QColor color)
{
    if (m_ballLists.find(ballList) != m_ballLists.end())
        return;

    BallListDataPtr data(new BallListData(ballList, color));
    m_ballLists[ballList] = data;

    updateGL();
}

void RenderView::removeDecimalBallList(DecimalBallListPtr ballList)
{
    BallLists::iterator it = m_ballLists.find(ballList);

    if (it == m_ballLists.end())
        return;

    m_ballLists.erase(it);

    updateGL();
}

void RenderView::setDecimalBallListColor(DecimalBallListPtr ballList, const QColor &color)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return;

    m_ballLists[ballList]->setColor(color);

    updateGL();
}

QColor RenderView::decimalBallListColor(DecimalBallListPtr ballList)
{
    if (m_ballLists.find(ballList) == m_ballLists.end())
        return QColor();

    return m_ballLists[ballList]->color();
}

void RenderView::addConfigurationSpace(ConfigurationSpacePtr configurationSpace)
{
    m_configurationSpaces.insert(configurationSpace);
    updateGL();
}

void RenderView::removeConfigurationSpace(ConfigurationSpacePtr configurationSpace)
{
    ConfigurationSpaces::iterator it = m_configurationSpaces.find(configurationSpace);

    if (it == m_configurationSpaces.end())
        return;

    m_configurationSpaces.erase(it);

    updateGL();
}

void RenderView::removeAllObjects()
{
    // remove render objects
    m_triangleLists.clear();
    m_spinLists.clear();
    m_spins.clear();
    m_ballLists.clear();
    m_configurationSpaces.clear();

    // update all
    updateGL();
}

TriangleListData::TriangleListData(QGLWidget *gl, DecimalTriangleListPtr triangleList, QColor color)
    : m_renderObject(gl, triangleList),
      m_color(color)
{
}

void TriangleListData::render(bool wireframe)
{
    // check visibility
    if (!m_visible)
        return;

    // setup material
    glColor3ub(m_color.red(), m_color.green(), m_color.blue());

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // draw geometry
    glPushMatrix();
        glTranslated(m_translation.x(), m_translation.y(), m_translation.z());
        applyRotationGL(m_quaternion);
        m_renderObject.render();
    glPopMatrix();

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TriangleListData::setColor(QColor color)
{
    m_color = color;
}

QColor TriangleListData::color() const
{
    return m_color;
}

void MeshData::setVisible(bool visible)
{
    m_visible = visible;
}

bool MeshData::isVisible() const
{
    return m_visible;
}

void MeshData::setTranslation(const QVector3D &translation)
{
    m_translation = translation;
}

QVector3D MeshData::translation() const
{
    return m_translation;
}

void MeshData::setQuaternion(const QQuaternion &quaternion)
{
    m_quaternion = quaternion;
}

QQuaternion MeshData::quaternion() const
{
    return m_quaternion;
}

MeshData::MeshData()
    : m_visible(true),
      m_translation(QVector3D()),
      m_quaternion(QQuaternion())
{
}

MeshData::~MeshData()
{
}

SpinListData::SpinListData(QGLWidget *gl, Qsic_spin_list_3_Z_ptr spinList)
    : m_renderObject(gl, spinList, QSIC_RADIUS, QSIC_SIDES)
{
}

void SpinListData::render()
{
    // check visibility
    if (!m_visible)
        return;

    // setup material
    glColor3ub(0, 127, 255);

    // draw geometry
    glPushMatrix();
        glTranslated(m_translation.x(), m_translation.y(), m_translation.z());
        m_renderObject.render();
    glPopMatrix();
}

BallListData::BallListData(DecimalBallListPtr ballList, QColor color)
    : m_ballList(ballList),
      m_color(color)
{
}

void BallListData::render(BallMesh *ball)
{
    // check visibility
    if (!m_visible)
        return;

    // setup material
    glColor3ub(m_color.red(), m_color.green(), m_color.blue());

    glPushMatrix();
        glTranslated(m_translation.x(), m_translation.y(), m_translation.z());
        applyRotationGL(m_quaternion);

        // draw geometry
        for (DecimalBallList::const_iterator it = m_ballList->begin(); it != m_ballList->end(); ++it)
        {
            glPushMatrix();
                glTranslated(it->center().x().toDouble(), it->center().y().toDouble(), it->center().z().toDouble());
                glScaled(it->radius().toDouble(), it->radius().toDouble(), it->radius().toDouble());
                applyRotationGL(m_quaternion);
                ball->render();
            glPopMatrix();
        }
    glPopMatrix();
}

void BallListData::setColor(QColor color)
{
    m_color = color;
}

QColor BallListData::color() const
{
    return m_color;
}
