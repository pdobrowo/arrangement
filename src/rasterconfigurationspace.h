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
#ifndef RASTERCONFIGURATIONSPACE_H
#define RASTERCONFIGURATIONSPACE_H

#include <cs/Voxel_3.h>
#include "configurationspace.h"
#include "ispoweroftwo.h"
#include "compressor.h"
#include "genericrouter.h"
#include "volumerenderer.h"
#include "volumerenderertexture3d.h"
#include "volumerenderergaussiansplatter.h"
#include <QDataStream>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>

class QGLWidget;

// volume renderer
enum VolumeRendererType
{
    VolumeRendererType_Texture3D,
    VolumeRendererType_GaussianSplatter
};

template<class Tag_>
struct RasterConfigurationSpaceTag
{
    typedef Tag_ Tag;
};

class RasterConfigurationSpace
    : public ConfigurationSpace
{
public:
    template<class Configuration_, typename InputIterator>
    RasterConfigurationSpace(const RasterConfigurationSpaceTag<Configuration_> &,
                             InputIterator robot_begin, InputIterator robot_end,
                             InputIterator obstacle_begin, InputIterator obstacle_end,
                             const typename Configuration_::Parameters &parameters,
                             VolumeRendererType volumeRendererType,
                             QGLWidget *gl)
        : ConfigurationSpace(gl)
    {
        typedef Configuration_                          Configuration;
        typedef typename Configuration::Parameters      Parameters;
        typedef typename Configuration::Representation  Representation;

        // allow only resolutions which are powers of two (for 3d texture)
        if (!isPowerOfTwo(parameters.resolution()))
            throw std::runtime_error("RasterConfigurationSpace: invalid parameters");

        // create configuration space for given representation
        GenericRouter<Configuration> *rasterRouter = new GenericRouter<Configuration>();

        rasterRouter->configuration().create_from_scene(robot_begin, robot_end,
                                                        obstacle_begin, obstacle_end,
                                                        parameters);

        // assume that the representation is raster
        const Representation &rep = rasterRouter->configuration().rep();

        // prepare voxels
        m_resolution = rep.resolution();
        m_voxels.reset(new VoxelType[m_resolution * m_resolution * m_resolution]);

        size_t index = 0;

        // scan points
        for (size_t u = 0; u < m_resolution; ++u)
        {
            for (size_t v = 0; v < m_resolution; ++v)
            {
                for (size_t w = 0; w < m_resolution; ++w)
                {
                    if (u == 0 || v == 0 || w == 0 || u == m_resolution - 1 || v == m_resolution - 1 || w == m_resolution - 1)
                    {
                        m_voxels[index++] = VoxelType_Border;
                    }
                    else
                    {
                        const typename Representation::Voxel &voxel = rep.voxel(u, v, w);

                        if (!voxel.is_real())
                        {
                            m_voxels[index++] = VoxelType_Imaginary;
                        }
                        else
                        {
                            if (!voxel.value(CS::Cover_Negative) &&
                                !voxel.value(CS::Cover_Positive))
                            {
                                // empty
                                m_voxels[index++] = VoxelType_Real_Empty;
                            }
                            else if (voxel.value(CS::Cover_Negative) &&
                                     voxel.value(CS::Cover_Positive))
                            {
                                // full
                                m_voxels[index++] = VoxelType_Real_Full;
                            }
                            else
                            {
                                // mixed
                                m_voxels[index++] = VoxelType_Real_Mixed;
                            }
                        }
                    }
                }
            }
        }

        switch (volumeRendererType)
        {
        case VolumeRendererType_Texture3D:
            m_volumeRenderer.reset(new VolumeRendererTexture3D(m_voxels.get(), m_resolution, m_gl));
            break;

        case VolumeRendererType_GaussianSplatter:
            m_volumeRenderer.reset(new VolumeRendererGaussianSplatter(m_voxels.get(), m_resolution, m_gl));
            break;
        }

        // install route executor
        m_router.reset(rasterRouter);
    }

    RasterConfigurationSpace(
            QDataStream &stream,
            VolumeRendererType volumeRendererType,
            QGLWidget *gl)
        : ConfigurationSpace(gl)
    {
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

        m_voxels.reset(new VoxelType[uncompressed.size()]);
        memcpy(m_voxels.get(), uncompressed.c_str(), uncompressed.size());

        switch (volumeRendererType)
        {
        case VolumeRendererType_Texture3D:
            m_volumeRenderer.reset(new VolumeRendererTexture3D(m_voxels.get(), m_resolution, m_gl));
            break;

        case VolumeRendererType_GaussianSplatter:
            m_volumeRenderer.reset(new VolumeRendererGaussianSplatter(m_voxels.get(), m_resolution, m_gl));
            break;
        }

        // note: there is no router for a pre-processed raster configuration space
    }

    virtual void render()
    {
        m_volumeRenderer->render();
    }

    virtual bool saveToStream(QDataStream &stream)
    {
        stream << static_cast<uint>(m_resolution);

        if (stream.status() != QDataStream::Ok)
            return false;

        std::string uncompressed(reinterpret_cast<char *>(m_voxels.get()),
                                 reinterpret_cast<char *>(m_voxels.get()) + m_resolution * m_resolution * m_resolution * sizeof(VoxelType));

        std::string compressed;
        Compressor::compress(uncompressed, compressed);

        stream.writeBytes(compressed.c_str(), static_cast<uint>(compressed.size()));

        return stream.status() == QDataStream::Ok;
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
    boost::scoped_ptr<VolumeRenderer>   m_volumeRenderer;
    size_t                              m_resolution;
    boost::scoped_array<VoxelType>      m_voxels;
};

typedef boost::shared_ptr<RasterConfigurationSpace> RasterConfigurationSpacePtr;

#endif // RASTERCONFIGURATIONSPACE_H
