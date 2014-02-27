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
#ifndef CELLCONFIGURATIONSPACE_H
#define CELLCONFIGURATIONSPACE_H

#include "configurationspace.h"
#include "genericrouter.h"
#include "volumerenderergaussiansplatter.h"
#include <QDataStream>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdexcept>

class QGLWidget;

template<class Tag_>
struct CellConfigurationSpaceTag
{
    typedef Tag_ Tag;
};

class CellConfigurationSpace
    : public ConfigurationSpace
{
public:
    template<class Configuration_, typename InputIterator>
    CellConfigurationSpace(const CellConfigurationSpaceTag<Configuration_> &,
                           InputIterator robot_begin, InputIterator robot_end,
                           InputIterator obstacle_begin, InputIterator obstacle_end,
                           const typename Configuration_::Parameters &parameters,
                           QGLWidget *gl)
        : ConfigurationSpace(gl)
    {
        typedef Configuration_                                  Configuration;
        typedef typename Configuration::Parameters              Parameters;
        typedef typename Configuration::Representation          Representation;
        typedef typename Representation::Cell_const_iterator    Cell_const_iterator;

        // create configuration space for given representation
        GenericRouter<Configuration> *cellRouter = new GenericRouter<Configuration>();

        cellRouter->configuration().create_from_scene(robot_begin, robot_end,
                                                      obstacle_begin, obstacle_end,
                                                      parameters);

        // assume that the representation is cell graph
        const Representation &rep = cellRouter->configuration().rep();

        // prepare
        size_t numberOfVoxels = parameters.sample_point_count();
        boost::scoped_array<Voxel> voxels(new Voxel[numberOfVoxels]);
        size_t index = 0;

        // scan points
        for (Cell_const_iterator cellIterator = rep.cells_begin(); cellIterator != rep.cells_end(); ++cellIterator)
        {
            typedef typename Representation::Cell::Sample_const_iterator Sample_const_iterator;
            VoxelType voxelType;

            // FIXME: take only one half-sphere !!! Do not mix them
            if (cellIterator->is_empty())
                voxelType = VoxelType_Real_Empty;
            else
                voxelType = VoxelType_Real_Full;

            for (Sample_const_iterator sampleIterator = cellIterator->samples_begin(); sampleIterator != cellIterator->samples_end(); ++sampleIterator)
            {
                assert(index < numberOfVoxels);
                voxels[index++] = Voxel(voxelType,
                                        sampleIterator->s12(),
                                        sampleIterator->s23(),
                                        sampleIterator->s31());
            }
        }

        m_volumeRendererGaussianSplatter.reset(new VolumeRendererGaussianSplatter(voxels.get(), voxels.get() + numberOfVoxels, m_gl));

        // install route executor
        m_router.reset(cellRouter);
    }

    CellConfigurationSpace(
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
        m_volumeRendererGaussianSplatter->render();
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
    boost::scoped_ptr<VolumeRendererGaussianSplatter>   m_volumeRendererGaussianSplatter;
};

typedef boost::shared_ptr<CellConfigurationSpace> CellConfigurationSpacePtr;

#endif // CELLCONFIGURATIONSPACE_H
