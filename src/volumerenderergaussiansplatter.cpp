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
#include "volumerenderergaussiansplatter.h"
#include "material.h"
#include <vtkPolyDataAlgorithm.h>
#include <vtkCellArray.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkCell.h>
#include <vtkSmartPointer.h>
#include <vtkGaussianSplatter.h>
#include <vtkContourFilter.h>
#include <boost/scoped_array.hpp>
#include <GL/glew.h>
#include <ostream>

class VTK_IO_EXPORT VoxelGridReader
    : public vtkPolyDataAlgorithm
{
public:
    static VoxelGridReader* New();
    vtkTypeMacro(VoxelGridReader, vtkPolyDataAlgorithm)
    void PrintSelf(std::ostream &os, vtkIndent indent);

    vtkSetMacro(Voxels, const VoxelType *)
    vtkGetMacro(Voxels, const VoxelType *)

    vtkSetMacro(Resolution, size_t)
    vtkGetMacro(Resolution, size_t)

    vtkSetMacro(Type, VoxelType)
    vtkGetMacro(Type, VoxelType)

protected:
    VoxelGridReader();
    ~VoxelGridReader();

    const VoxelType *   Voxels;
    size_t              Resolution;
    VoxelType           Type;

    int RequestData(vtkInformation *,
                    vtkInformationVector **,
                    vtkInformationVector *);
private:
    VoxelGridReader(const VoxelGridReader &);
    void operator=(const VoxelGridReader &);
};

vtkStandardNewMacro(VoxelGridReader)

VoxelGridReader::VoxelGridReader()
{
    Voxels = 0;
    Resolution = 0;
    SetNumberOfInputPorts(0);
}

VoxelGridReader::~VoxelGridReader()
{
}

void VoxelGridReader::PrintSelf(ostream& os, vtkIndent indent)
{
    Superclass::PrintSelf(os,indent);
}

int VoxelGridReader::RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *outputVector)
{
    // Make sure we have a file to read.
    if (!Voxels || !Resolution)
    {
        vtkErrorMacro("No texels or resolution specified");
        return 0;
    }

    // Allocate objects to hold points and vertex cells.
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();

    // Copy texels to points
    for (size_t u = 0; u < Resolution; ++u)
    {
        for (size_t v = 0; v < Resolution; ++v)
        {
            for (size_t w = 0; w < Resolution; ++w)
            {
                // calculate random sample
                double s12 = 2.0 * double(u) / double(Resolution - 1) - 1.0;
                double s23 = 2.0 * double(v) / double(Resolution - 1) - 1.0;
                double s31 = 2.0 * double(w) / double(Resolution - 1) - 1.0;

                VoxelType voxelType = Voxels[Resolution * (Resolution * w + v) + u];

                if (voxelType == Type)
                {
                    vtkIdType id = points->InsertNextPoint(s12, s23, s31);
                    cellArray->InsertNextCell(1, &id);
                }
            }
        }
    }

    // Store the points and cells in the output data object.
    vtkPolyData* output = vtkPolyData::GetData(outputVector);
    output->SetPoints(points);
    output->SetVerts(cellArray);

    return 1;
}

class VTK_IO_EXPORT VoxelListReader
    : public vtkPolyDataAlgorithm
{
public:
    static VoxelListReader* New();
    vtkTypeMacro(VoxelListReader, vtkPolyDataAlgorithm)
    void PrintSelf(std::ostream &os, vtkIndent indent);

    vtkSetMacro(VoxelsBegin, const Voxel *)
    vtkGetMacro(VoxelsBegin, const Voxel *)

    vtkSetMacro(VoxelsEnd, const Voxel *)
    vtkGetMacro(VoxelsEnd, const Voxel *)

    vtkSetMacro(Type, VoxelType)
    vtkGetMacro(Type, VoxelType)

protected:
    VoxelListReader();
    ~VoxelListReader();

    const Voxel *       VoxelsBegin;
    const Voxel *       VoxelsEnd;
    VoxelType           Type;

    int RequestData(vtkInformation *,
                    vtkInformationVector **,
                    vtkInformationVector *);
private:
    VoxelListReader(const VoxelListReader &);
    void operator=(const VoxelListReader &);
};

vtkStandardNewMacro(VoxelListReader)

VoxelListReader::VoxelListReader()
{
    VoxelsBegin = 0;
    VoxelsEnd = 0;
    SetNumberOfInputPorts(0);
}

VoxelListReader::~VoxelListReader()
{
}

void VoxelListReader::PrintSelf(ostream& os, vtkIndent indent)
{
    Superclass::PrintSelf(os,indent);
}

int VoxelListReader::RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *outputVector)
{
    // Make sure we have a file to read.
    if (!VoxelsBegin || !VoxelsEnd)
    {
        vtkErrorMacro("No voxel begin or voxel end specified");
        return 0;
    }

    // Allocate objects to hold points and vertex cells.
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();

    // Copy texels to points
    const Voxel *current = VoxelsBegin;

    while (current != VoxelsEnd)
    {
        if (current->type == Type)
        {
            vtkIdType id = points->InsertNextPoint(current->x, current->y, current->z);
            cellArray->InsertNextCell(1, &id);
        }

        ++current;
    }

    // Store the points and cells in the output data object.
    vtkPolyData* output = vtkPolyData::GetData(outputVector);
    output->SetPoints(points);
    output->SetVerts(cellArray);

    return 1;
}

VolumeRendererGaussianSplatter::VolumeRendererGaussianSplatter(const VoxelType *voxels, size_t resolution, QGLWidget *gl)
{
    // prepare data sources
    vtkSmartPointer<VoxelGridReader> dataSourceRealFull = vtkSmartPointer<VoxelGridReader>::New();
    dataSourceRealFull->SetVoxels(voxels);
    dataSourceRealFull->SetType(VoxelType_Real_Full);
    dataSourceRealFull->SetResolution(resolution);
    dataSourceRealFull->Update();

    vtkSmartPointer<VoxelGridReader> dataSourceRealMixed = vtkSmartPointer<VoxelGridReader>::New();
    dataSourceRealMixed->SetVoxels(voxels);
    dataSourceRealMixed->SetType(VoxelType_Real_Mixed);
    dataSourceRealMixed->SetResolution(resolution);
    dataSourceRealMixed->Update();

    // create triangle lists
    m_triangleListMeshFull = meshVoxels(dataSourceRealFull, gl);
    m_triangleListMeshMixed = meshVoxels(dataSourceRealMixed, gl);
}

VolumeRendererGaussianSplatter::VolumeRendererGaussianSplatter(const Voxel *begin, const Voxel *end, QGLWidget *gl)
{
    // prepare data sources
    vtkSmartPointer<VoxelListReader> dataSourceRealFull = vtkSmartPointer<VoxelListReader>::New();
    dataSourceRealFull->SetVoxelsBegin(begin);
    dataSourceRealFull->SetVoxelsEnd(end);
    dataSourceRealFull->SetType(VoxelType_Real_Full);
    dataSourceRealFull->Update();

    vtkSmartPointer<VoxelListReader> dataSourceRealMixed = vtkSmartPointer<VoxelListReader>::New();
    dataSourceRealMixed->SetVoxelsBegin(begin);
    dataSourceRealMixed->SetVoxelsEnd(end);
    dataSourceRealMixed->SetType(VoxelType_Real_Mixed);
    dataSourceRealMixed->Update();

    // create triangle lists
    m_triangleListMeshFull = meshVoxels(dataSourceRealFull, gl);
    m_triangleListMeshMixed = meshVoxels(dataSourceRealMixed, gl);
}

TriangleListMeshPtr VolumeRendererGaussianSplatter::meshVoxels(vtkPolyDataAlgorithm *dataSource, QGLWidget *gl)
{
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(dataSource->GetOutput()->GetPoints());

    vtkSmartPointer<vtkGaussianSplatter> gaussianSplatter = vtkSmartPointer<vtkGaussianSplatter>::New();
    gaussianSplatter->SetInput(polyData);
    gaussianSplatter->SetSampleDimensions(200, 200, 200);
    gaussianSplatter->SetRadius(0.02);
    gaussianSplatter->ScalarWarpingOff();

    vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
    contourFilter->SetInputConnection(gaussianSplatter->GetOutputPort());

    int numberOfContours = contourFilter->GetNumberOfContours();

    for (int i = 0; i < numberOfContours; ++i)
        contourFilter->SetValue(i, 0.01);

    contourFilter->UseScalarTreeOn();

    // compute
    contourFilter->Update();

    // parse output
    vtkPolyData *outputPolyData = contourFilter->GetOutput();
    vtkIdType numberOfCells = outputPolyData->GetNumberOfCells();

    Mesh_smooth_triangle_list_3_Z_ptr smoothTriangleList(new Mesh_smooth_triangle_list_3_Z());

    for (vtkIdType i = 0; i < numberOfCells; ++i)
    {
        vtkCell *cell = outputPolyData->GetCell(i);

        int dimension = cell->GetCellDimension();

        if (dimension == 2)
        {
            vtkIdType numberOfCellPoints = cell->GetNumberOfPoints();
            boost::scoped_array<vtkIdType> cellPointIds(new vtkIdType[numberOfCellPoints]);

            for (vtkIdType p = 0; p < numberOfCellPoints; ++p)
                cellPointIds[p] = cell->GetPointId(p);

            // triangulate cell
            vtkSmartPointer<vtkIdList> idList = vtkIdList::New();
            vtkSmartPointer<vtkPoints> points = vtkPoints::New();

            int result = cell->Triangulate(0, idList, points);

            if (result == 1)
            {
                vtkIdType numberOfIds = idList->GetNumberOfIds();
                vtkIdType j = 0;

                while (j < numberOfIds)
                {
                    vtkIdType indexA = std::find(cellPointIds.get(), cellPointIds.get() + numberOfCellPoints, idList->GetId(j++)) - cellPointIds.get();
                    vtkIdType indexB = std::find(cellPointIds.get(), cellPointIds.get() + numberOfCellPoints, idList->GetId(j++)) - cellPointIds.get();
                    vtkIdType indexC = std::find(cellPointIds.get(), cellPointIds.get() + numberOfCellPoints, idList->GetId(j++)) - cellPointIds.get();

                    double a[3], b[3], c[3];
                    cell->GetPoints()->GetPoint(indexA, a);
                    cell->GetPoints()->GetPoint(indexB, b);
                    cell->GetPoints()->GetPoint(indexC, c);

                    Mesh_smooth_triangle_3_Z::Vector_3 normal = CGAL::cross_product(
                                Mesh_smooth_triangle_3_Z::Vector_3(b[0] - a[0], b[1] - a[1], b[2] - a[2]),
                                Mesh_smooth_triangle_3_Z::Vector_3(c[0] - a[0], c[1] - a[1], c[2] - a[2]));

                    Mesh_smooth_triangle_3_Z smoothTriangle(Mesh_smooth_triangle_3_Z::Triangle_3(
                                                                Mesh_smooth_triangle_3_Z::Point_3(a[0], a[1], a[2]),
                                                                Mesh_smooth_triangle_3_Z::Point_3(b[0], b[1], b[2]),
                                                                Mesh_smooth_triangle_3_Z::Point_3(c[0], c[1], c[2])),
                                                            normal, normal, normal);

                    smoothTriangleList->push_back(smoothTriangle);
                }
            }
        }
    }

    return TriangleListMeshPtr(new TriangleListMesh(gl, smoothTriangleList));
}

void VolumeRendererGaussianSplatter::render()
{
    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

    GLdouble camera[3] = { -(modelview[0] * modelview[12] + modelview[1] * modelview[13] + modelview[2] * modelview[14]),
                           -(modelview[4] * modelview[12] + modelview[5] * modelview[13] + modelview[6] * modelview[14]),
                           -(modelview[8] * modelview[12] + modelview[9] * modelview[13] + modelview[10] * modelview[14]) };

    GLdouble clipPlane[4] = { -modelview[2], -modelview[6], -modelview[10], 0 };
    GLdouble clipPoint[3] = { camera[0] - 3 * modelview[2], camera[1] - 3 * modelview[6], camera[2] - 3 * modelview[10] };

    clipPlane[3] = -(clipPoint[0] * clipPlane[0] + clipPoint[1] * clipPlane[1] + clipPoint[2] * clipPlane[2]);

    // enable cube clip plane
    glClipPlane(GL_CLIP_PLANE0 + 0, clipPlane);
    glEnable(GL_CLIP_PLANE0 + 0);

    // draw triangle mesh
    Material::setDiffuseSpecularShininess(Qt::red);
    m_triangleListMeshFull->render();

    Material::setDiffuseSpecularShininess(Qt::yellow);
    m_triangleListMeshMixed->render();

    // disable cube clip plane
    glDisable(GL_CLIP_PLANE0 + 0);
}
