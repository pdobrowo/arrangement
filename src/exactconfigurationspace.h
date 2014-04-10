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
#ifndef EXACTCONFIGURATIONSPACE_H
#define EXACTCONFIGURATIONSPACE_H

#include "configurationspace.h"
#include "genericrouter.h"
#include "trianglelistmesh.h"
#include "polyconemesh.h"
#include "material.h"
#include <QDataStream>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdexcept>

class QGLWidget;

template<class Tag_>
struct ExactConfigurationSpaceTag
{
    typedef Tag_ Tag;
};

class ExactConfigurationSpace
    : public ConfigurationSpace
{
public:
    template<class Configuration_, typename InputIterator>
    ExactConfigurationSpace(const ExactConfigurationSpaceTag<Configuration_> &,
                           InputIterator robot_begin, InputIterator robot_end,
                           InputIterator obstacle_begin, InputIterator obstacle_end,
                           const typename Configuration_::Parameters &parameters,
                           QGLWidget *gl)
        : ConfigurationSpace(gl)
    {
        typedef Configuration_                                  Configuration;
        //typedef typename Configuration::Parameters              Parameters;
        typedef typename Configuration::Representation          Representation;

        // create configuration space for given representation
        GenericRouter<Configuration> *exactRouter = new GenericRouter<Configuration>();

        exactRouter->configuration().create_from_scene(robot_begin, robot_end,
                                                       obstacle_begin, obstacle_end,
                                                       parameters);

        // assume that the representation is cell graph
        const Representation &rep = exactRouter->configuration().rep();

        // prepare
        typedef typename Representation::Spin_quadric_const_iterator Spin_quadric_const_iterator;

        for (Spin_quadric_const_iterator spinQuadricIterator = rep.spin_quadrics_begin();
             spinQuadricIterator != rep.spin_quadrics_end(); ++spinQuadricIterator)
        {
            Spin_quadric_mesh_3_Z mesher(*spinQuadricIterator);

            double angular_bound = 30;
            double radius_bound = 0.03;
            double distance_bound = 0.03;

            Mesh_smooth_triangle_list_3_Z_ptr left(new Mesh_smooth_triangle_list_3_Z());
            Mesh_smooth_triangle_list_3_Z_ptr right(new Mesh_smooth_triangle_list_3_Z());

            mesher.mesh_triangle_soup(std::back_inserter(*left),
                                      std::back_inserter(*right),
                                      angular_bound,
                                      radius_bound,
                                      distance_bound);

            // create triangle lists
            TriangleListMeshPtr leftMesh(new TriangleListMesh(m_gl, left));
            TriangleListMeshPtr rightMesh(new TriangleListMesh(m_gl, right));

            // store triangle lists
            m_triangleListMeshPairs.push_back(std::make_pair(leftMesh, rightMesh));
        }

        // QSICs
        typedef typename Representation::Qsic_const_iterator Qsic_const_iterator;
        typedef typename Representation::Qsic_handle Qsic_handle;

        for (Qsic_const_iterator qsicIterator = rep.qsics_begin();
             qsicIterator != rep.qsics_end(); ++qsicIterator)
        {
            Qsic_handle qsic = *qsicIterator;
            Spin_qsic_mesh_3_Z mesher(*qsic);

            // each component
            for (size_t component = 0; component != mesher.size_of_components(); ++component)
            {
                // FIXME: if the component is not one dimensional, ignore it
                if (qsic->component_dimension(component) != 1)
                    continue;

                // evaluate curve
                Qsic_spin_list_3_Z_ptr spinList(new Qsic_spin_list_3_Z());

                double radiusBound = 0.1;

                mesher.mesh_component(*spinList, component, radiusBound);

                Qsic_spin_list_3_Z_ptr negSpinList(new Qsic_spin_list_3_Z());

                foreach (const Qsic_spin_3_Z &spin, *spinList)
                    negSpinList->push_back(-spin);

                // create poly cones
                PolyConeMeshPtr positivePolyCone(new PolyConeMesh(m_gl, spinList, 0.05, 12));
                PolyConeMeshPtr negativePolyCone(new PolyConeMesh(m_gl, negSpinList, 0.05, 12));

                // store poly cones
                m_polyConeMeshPairs.push_back(std::make_pair(positivePolyCone, negativePolyCone));
            }
        }

        // install route executor
        m_router.reset(exactRouter);
    }

    ExactConfigurationSpace(
            QDataStream &stream,
            QGLWidget *gl)
        : ConfigurationSpace(gl)
    {
        Q_UNUSED(stream);
/*
        // read a compressed raster configuration space
        uint resolution;
        stream >> resolution;

        if (stream.status() != QDataStream::Ok)
            throw std::runtime_error("Failed to load configuration space!");

        m_resolution = static_cast<size_t>(resolution);

        char *data;
        uint length;

        stream.readBytes(data, length);

        if (stream.status() != QDataStream::Ok)
            throw std::runtime_error("Failed to load configuration space!");

        std::string compressed(data, data + length);

        std::string uncompressed;
        Compressor::decompress(compressed, uncompressed);

        m_data.reset(new unsigned char[uncompressed.size()]);
        memcpy(m_data.get(), uncompressed.c_str(), uncompressed.size());

        m_boxTomograph.reset(new BoxTomograph(m_data.get(), m_resolution));

        // note: there is no router for a pre-processed raster configuration space
*/
    }

    virtual void render()
    {
        enableViewClipPlane();

        Material::setDiffuseSpecularShininess(QColor(238, 144, 20));

        // render spin quadrics
        for (TriangleListMeshPairs::iterator triangleListMeshIterator = m_triangleListMeshPairs.begin();
             triangleListMeshIterator != m_triangleListMeshPairs.end(); ++triangleListMeshIterator)
        {
            triangleListMeshIterator->first->render();
            triangleListMeshIterator->second->render();
        }

        Material::setDiffuseSpecularShininess(QColor(51, 147, 41));

        // render poly cones
        for (PolyConeMeshPairs::iterator polyConeIterator = m_polyConeMeshPairs.begin();
             polyConeIterator != m_polyConeMeshPairs.end(); ++polyConeIterator)
        {
            polyConeIterator->first->render();
            polyConeIterator->second->render();
        }

        disableViewClipPlane();
    }

    virtual bool saveToStream(QDataStream &stream)
    {
        Q_UNUSED(stream);
        return false;
/*
        stream << static_cast<uint>(m_resolution);

        if (stream.status() != QDataStream::Ok)
            return false;

        std::string uncompressed(reinterpret_cast<char *>(m_data.get()),
                                 reinterpret_cast<char *>(m_data.get()) + m_resolution * m_resolution * m_resolution * 3);

        std::string compressed;
        Compressor::compress(uncompressed, compressed);

        stream.writeBytes(compressed.c_str(), static_cast<uint>(compressed.size()));

        return stream.status() == QDataStream::Ok;
*/
    }

    virtual bool needsLighting() const
    {
        return true;
    }

    virtual bool needsShaderLighting() const
    {
        return false;
    }

private:
    typedef std::vector<std::pair<TriangleListMeshPtr, TriangleListMeshPtr> > TriangleListMeshPairs;
    typedef std::vector<std::pair<PolyConeMeshPtr, PolyConeMeshPtr> > PolyConeMeshPairs;

    TriangleListMeshPairs   m_triangleListMeshPairs;
    PolyConeMeshPairs       m_polyConeMeshPairs;
};

typedef boost::shared_ptr<ExactConfigurationSpace> ExactConfigurationSpacePtr;

#endif // EXACTCONFIGURATIONSPACE_H
