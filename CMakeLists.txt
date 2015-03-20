###
# Copyright (C) 2009-2015  Przemysław Dobrowolski
#
# This file is part of the Configuration Space Library (libcs), a library
# for creating configuration spaces of various motion planning problems.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
PROJECT(arrangement)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

# cmake find
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../libcs/cmake)

# settings
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)

# qt5
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

# cs
FIND_PACKAGE(CS REQUIRED)
INCLUDE_DIRECTORIES(${CS_INCLUDE_DIR})

# qdecimal
INCLUDE_DIRECTORIES(${CS_3RD_INCLUDE_DIR}/qdecimal/include)
LINK_DIRECTORIES(${CS_3RD_INCLUDE_DIR}/qdecimal/lib)

# vtk
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# compiler settings
SET(ARRANGEMENT_DEFAULT_CLANG_AND_GNU_FLAGS "-std=c++11 -Wall -Wextra")

IF(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
ADD_DEFINITIONS(${ARRANGEMENT_DEFAULT_CLANG_AND_GNU_FLAGS})
ENDIF(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)

IF(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
ADD_DEFINITIONS(${ARRANGEMENT_DEFAULT_CLANG_AND_GNU_FLAGS})
ENDIF(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)

# output directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# headers
SET(arrangement_HEADERS
    src/aboutdialog.h
    src/ballmesh.h
    src/benchmarkdialog.h
    src/cellconfigurationspace.h
    src/clientform.h
    src/colorwidget.h
    src/compressor.h
    src/configurationobjectdialog.h
    src/configurationobject.h
    src/configurationspace.h
    src/decimalscene.h
    src/exactconfigurationspace.h
    src/genericrouter.h
    src/gridmesh.h
    src/ispoweroftwo.h
    src/kernel.h
    src/logobackform.h
    src/mainwindow.h
    src/material.h
    src/mesh.h
    src/multisplitter.h
    src/numbervalidator.h
    src/planevalidator.h
    src/pointlistmesh.h
    src/polyconemesh.h
    src/predicateg.h
    src/predicateh.h
    src/predicates.h
    src/qdecimal.h
    src/qlog4cxx.h
    src/rasterconfigurationspace.h
    src/renderviewarcballcamera.h
    src/renderviewautocamera.h
    src/renderviewcamera.h
    src/renderviewflycamera.h
    src/renderview.h
    src/sampledroute.h
    src/sceneloader.h
    src/sceneobjectdialog.h
    src/sceneobject.h
    src/scopeddisablelighting.h
    src/shader.h
    src/spheretreeloader.h
    src/spin3.h
    src/triangleintersectiondialog.h
    src/trianglelistmesh.h
    src/tritripredicatelist.h
    src/variantpredicatedialog.h
    src/variantpredicate.h
    src/vectorvalidator.h
    src/volumerenderergaussiansplatter.h
    src/volumerenderer.h
    src/volumerenderertexture3d.h
)

# sources
SET(arrangement_SOURCES
    ${arrangement_HEADERS}
    src/aboutdialog.cpp
    src/ballmesh.cpp
    src/benchmarkdialog.cpp
    src/clientform.cpp
    src/colorwidget.cpp
    src/compressor.cpp
    src/configurationobject.cpp
    src/configurationobjectdialog.cpp
    src/gridmesh.cpp
    src/logobackform.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/material.cpp
    src/mesh.cpp
    src/multisplitter.cpp
    src/numbervalidator.cpp
    src/planevalidator.cpp
    src/pointlistmesh.cpp
    src/polyconemesh.cpp
    src/predicateg.cpp
    src/predicateh.cpp
    src/predicates.cpp
    src/qlog4cxx.cpp
    src/rasterconfigurationspace.cpp
    src/renderviewarcballcamera.cpp
    src/renderviewautocamera.cpp
    src/renderview.cpp
    src/renderviewflycamera.cpp
    src/sceneloader.cpp
    src/sceneobject.cpp
    src/sceneobjectdialog.cpp
    src/scopeddisablelighting.cpp
    src/shader.cpp
    src/spheretreeloader.cpp
    src/spin3.cpp
    src/triangleintersectiondialog.cpp
    src/trianglelistmesh.cpp
    src/tritripredicatelist.cpp
    src/variantpredicate.cpp
    src/variantpredicatedialog.cpp
    src/vectorvalidator.cpp
    src/volumerenderergaussiansplatter.cpp
    src/volumerenderertexture3d.cpp
)

IF (WIN32)
    SET(arrangement_SOURCES
        windowsicon.rc
        ${arrangement_SOURCES}
    )
ENDIF(WIN32)


# resources
SET(arrangement_RESOURCES
    resources.qrc
)

# libs
SET(arrangement_LIBS
    # qt5
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::OpenGL

    # other
    qdecimal
    decnumber    

    # sys
    GL
    GLU
    gle
    GLEW
    log4cxx
    ${VTK_LIBRARIES}
)

# bin
add_executable(arrangement ${arrangement_SOURCES} ${arrangement_RESOURCES})
target_link_libraries(arrangement ${CS_LIBRARIES} ${arrangement_LIBS})

