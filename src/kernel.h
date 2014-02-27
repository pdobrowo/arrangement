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
#ifndef KERNEL_H
#define KERNEL_H

#include <CGAL/Cartesian.h>
#include <CGAL/Homogeneous.h>
#include <cs/Spin_kernel_3.h>
#include <cs/Bigint.h>
#include <boost/shared_ptr.hpp>
#include <CGAL/Polyhedron_3.h>

// Kernel is:
//
// * exact
// * filtered
// * cartesian
// * bigint based

// Kernel type
typedef CGAL::Bigint                                    Z;
typedef CGAL::Filtered_kernel<CGAL::Cartesian<Z> >      Kernel_Z_base;
typedef CS::Spin_kernel_3<Kernel_Z_base>                Kernel_Z;

// Real kernel type
typedef double                                          R;
typedef CGAL::Cartesian<R>                              Kernel_R_base;
typedef CS::Spin_inexact_kernel_3<Kernel_R_base>        Kernel_R;

// Base geometric types
typedef Kernel_Z::Vector_3                              Vector_3_Z;
typedef Kernel_Z::Point_3                               Point_3_Z;
typedef Kernel_Z::Plane_3                               Plane_3_Z;

typedef Kernel_R::Vector_3                              Vector_3_R;
typedef Kernel_R::Point_3                               Point_3_R;
typedef Kernel_R::Plane_3                               Plane_3_R;

// Predicate types
typedef Kernel_Z::Predicate_h_3                         Predicate_H_3_Z;
typedef boost::shared_ptr<Predicate_H_3_Z>              Predicate_H_3_Z_ptr;

typedef Kernel_Z::Predicate_s_3                         Predicate_S_3_Z;
typedef boost::shared_ptr<Predicate_S_3_Z>              Predicate_S_3_Z_ptr;

typedef Kernel_Z::Predicate_g_3                         Predicate_G_3_Z;
typedef boost::shared_ptr<Predicate_G_3_Z>              Predicate_G_3_Z_ptr;

typedef Kernel_Z::Predicate_tt_3                        Predicate_TT_3_Z;
typedef boost::shared_ptr<Predicate_TT_3_Z>             Predicate_TT_3_Z_ptr;

typedef Kernel_R::Predicate_tt_3                        Predicate_TT_3_R;
typedef boost::shared_ptr<Predicate_TT_3_R>             Predicate_TT_3_R_ptr;

typedef Kernel_Z::Predicate_bb_3                        Predicate_BB_3_Z;
typedef boost::shared_ptr<Predicate_BB_3_Z>             Predicate_BB_3_Z_ptr;

typedef Kernel_R::Predicate_bb_3                        Predicate_BB_3_R;
typedef boost::shared_ptr<Predicate_BB_3_R>             Predicate_BB_3_R_ptr;

// Spin configuration space types
typedef Kernel_Z::Spin_quadric_3                        Spin_quadric_3_Z;
typedef boost::shared_ptr<Spin_quadric_3_Z>             Spin_quadric_3_Z_ptr;

typedef Kernel_Z::Spin_qsic_3                           Spin_qsic_3_Z;
typedef boost::shared_ptr<Spin_qsic_3_Z>                Spin_qsic_3_Z_ptr;

typedef Kernel_Z::Spin_qsic_mesh_3                      Spin_qsic_mesh_3_Z;
typedef boost::shared_ptr<Spin_qsic_mesh_3_Z>           Spin_qsic_mesh_3_Z_ptr;

typedef Spin_qsic_mesh_3_Z::Spin_3                      Qsic_spin_3_Z;
typedef boost::shared_ptr<Qsic_spin_3_Z>                Qsic_spin_3_Z_ptr;

typedef Spin_qsic_mesh_3_Z::Spin_list_3                 Qsic_spin_list_3_Z;
typedef boost::shared_ptr<Qsic_spin_list_3_Z>           Qsic_spin_list_3_Z_ptr;

typedef Kernel_Z::Spin_quadric_mesh_3                   Spin_quadric_mesh_3_Z;

typedef Spin_quadric_mesh_3_Z::Polyhedron_3             Polyhedron_3_Z;
typedef boost::shared_ptr<Polyhedron_3_Z>               Polyhedron_3_Z_ptr;

typedef Spin_quadric_mesh_3_Z::Smooth_triangle_3        Mesh_smooth_triangle_3_Z;
typedef boost::shared_ptr<Mesh_smooth_triangle_3_Z>     Mesh_smooth_triangle_3_Z_ptr;

typedef Spin_quadric_mesh_3_Z::Point_3                  Mesh_point_3_Z;
typedef boost::shared_ptr<Mesh_point_3_Z>               Mesh_point_3_Z_ptr;

typedef std::vector<Mesh_smooth_triangle_3_Z>               Mesh_smooth_triangle_list_3_Z;
typedef boost::shared_ptr<Mesh_smooth_triangle_list_3_Z>    Mesh_smooth_triangle_list_3_Z_ptr;

typedef Kernel_Z::Spin_qsip_3                           Spin_qsip_3_Z;
typedef boost::shared_ptr<Spin_qsip_3_Z>                Spin_qsip_3_Z_ptr;

typedef Kernel_Z::Spin_qsip_mesh_3                      Spin_qsip_mesh_3_Z;
typedef boost::shared_ptr<Spin_qsip_mesh_3_Z>           Spin_qsip_mesh_3_Z_ptr;

typedef Spin_qsip_mesh_3_Z::Spin_3                      Qsip_spin_3_Z;
typedef boost::shared_ptr<Qsip_spin_3_Z>                Qsip_spin_3_Z_ptr;

typedef Kernel_R::Ball_3                                Ball_3_R;
typedef std::vector<Ball_3_R>                           Ball_list_3_R;
typedef boost::shared_ptr<Ball_list_3_R>                Ball_list_3_R_ptr;

typedef Kernel_Z::Ball_3                                Ball_3_Z;
typedef std::vector<Ball_3_Z>                           Ball_list_3_Z;
typedef boost::shared_ptr<Ball_list_3_Z>                Ball_list_3_Z_ptr;

typedef Kernel_R::Triangle_3                            Triangle_3_R;
typedef std::vector<Triangle_3_R>                       Triangle_list_3_R;
typedef boost::shared_ptr<Triangle_list_3_R>            Triangle_list_3_R_ptr;

typedef Kernel_Z::Triangle_3                            Triangle_3_Z;
typedef std::vector<Triangle_3_Z>                       Triangle_list_3_Z;
typedef boost::shared_ptr<Triangle_list_3_Z>            Triangle_list_3_Z_ptr;

// Configuration spaces
struct Spin_configuration_space_3
{
    typedef Kernel_Z::Spin_exact_configuration_space_3<Predicate_TT_3_Z>::Type    Exact_TT_Z;
    typedef Kernel_Z::Spin_exact_configuration_space_3<Predicate_BB_3_Z>::Type    Exact_BB_Z;
    typedef Kernel_Z::Spin_cell_configuration_space_3<Predicate_TT_3_Z>::Type     Cell_TT_Z;
    typedef Kernel_Z::Spin_cell_configuration_space_3<Predicate_BB_3_Z>::Type     Cell_BB_Z;
    typedef Kernel_Z::Spin_raster_configuration_space_3<Predicate_TT_3_Z>::Type   Raster_TT_Z;
    typedef Kernel_Z::Spin_raster_configuration_space_3<Predicate_BB_3_Z>::Type   Raster_BB_Z;

    typedef Kernel_R::Spin_exact_configuration_space_3<Predicate_TT_3_R>::Type    Exact_TT_R;
    typedef Kernel_R::Spin_exact_configuration_space_3<Predicate_BB_3_R>::Type    Exact_BB_R;
    typedef Kernel_R::Spin_cell_configuration_space_3<Predicate_TT_3_R>::Type     Cell_TT_R;
    typedef Kernel_R::Spin_cell_configuration_space_3<Predicate_BB_3_R>::Type     Cell_BB_R;
    typedef Kernel_R::Spin_raster_configuration_space_3<Predicate_TT_3_R>::Type   Raster_TT_R;
    typedef Kernel_R::Spin_raster_configuration_space_3<Predicate_BB_3_R>::Type   Raster_BB_R;
};

#endif // KERNEL_H
