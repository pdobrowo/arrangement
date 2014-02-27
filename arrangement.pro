#-------------------------------------------------
# arrangement
# author: pdobrowolski
#-------------------------------------------------
QT += core gui opengl

TARGET = arrangement
TEMPLATE = app
DESTDIR = ../bin

GXX_WARNING_FLAGS = \
#   -Werror                      \ # Make all warnings into errors.
    -Wall                        \ # Generate all warnings
#   -Wextra                      \ # Generate even more extra warnings
#   -pedantic                    \ # Accept only pedantic code
#   -Weffc++                     \ # Accept only effective C++ code
    -Wwrite-strings              \ # Do not accept writing to contant string memory
    -Winit-self                  \ # Do not accept initializing variable with itself
    -Wcast-align                 \ # Do not accept misaligning with casting
    -Wcast-qual                  \ # Do not accept removing qualifiers with casting
#   -Wold-style-cast             \ # Do not accept old style casting
    -Wpointer-arith              \ # Warn about void pointer arthimetic
    -Wstrict-aliasing            \ # Ensure strict aliasing
    -Wuninitialized              \ # Do not accept uninitialized variables
#   -Wmissing-declarations       \ # Warn about global and non-accesible functions
    -Woverloaded-virtual         \ # Warn about incidental overiding non-virtual base methods
    -Wnon-virtual-dtor           \ # Warn about non-virtual destructor
#   -Wctor-dtor-privacy          \ # Warn about useless and non-constructible classes
#   -Wlong-long                  \ # Do not allow using long long
#   -Wunreachable-code           \ # Warn about unreachable code
#   -Wfloat-equal                \ # Do not accept comparing floating points with equal operator
    -Wabi                        \ # Warn about possible ABI problems
#   -Wswitch-enum                \ # Check switch enumeration
    -Wformat=2                   \ # Check printf formatting
#   -Wundef                      \ # Warn if an undefined identifier is evaluated in an @if directive.
#   -Wshadow                     \ # Warn whenever a local variable shadows another local variable, parameter or global variable or whenever a built-in function is shadowed
#   -Wconversion                 \ # Warn for implicit conversions that may alter a value
#   -Wlogical-op                 \ # Warn about suspicious uses of logical operators in expressions
#   -Waggregate-return           \ # Warn if any functions that return structures or unions are defined or called.
    -Wmissing-field-initializers \ # Warn if a structure's initializer has some fields missing.
    -Wredundant-decls            \ # Warn if anything is declared more than once in the same scope, even in cases where multiple declaration is valid and changes nothing.
#   -Wmissing-include-dirs       \ # Warn if a user-supplied include directory does not exist.
#   -Wswitch-default             \ # Warn whenever a switch statement does not have a default case.
    -Wsync-nand                  \ # Warn when __sync_fetch_and_nand and __sync_nand_and_fetch built-in functions are used. These functions changed semantics in GCC 4.4.
    -Wunused                     \ # All the above -Wunused options combined.
#   -Wstrict-overflow=5          \ # Also warn about cases where the compiler reduces the magnitude of a constant involved in a comparison.
#   -Wunsafe-loop-optimizations  \ # Warn if the loop cannot be optimized because the compiler could not assume anything on the bounds of the loop indices. With -funsafe-loop-optimizations warn if the compiler made such assumptions.
    -Wmissing-format-attribute   \ # Warn about function pointers which might be candidates for format attributes.
#   -Wpadded                     \ # Warn if padding is included in a structure, either to align an element of the structure or to align the whole structure.
#   -Winline                     \ # Warn if a function can not be inlined and it was declared as inline.
    -Wdisabled-optimization      \ # Warn if a requested optimization pass is disabled.

#
# compiler
#
ARCH = "../3rdparty"

# architecture
contains(QMAKE_HOST.arch, x86_64):{
    ARCH = "$$ARCH/amd64"
} else {
    ARCH = "$$ARCH/i386"
}

# system
win32*:{
    ARCH = "$$ARCH/windows"
}
linux*:{
    ARCH = "$$ARCH/linux"
}

message(architecture: $$ARCH)

# lidia
LIDIA = "$$ARCH/lidia-2.3.0"
INCLUDEPATH += "$$LIDIA/include"
LIBS += -L"$$LIDIA/lib"

# libqi
LIBQI = "$$ARCH/libqi-0.9.33"
INCLUDEPATH += "$$LIBQI/include"
LIBS += -L"$$LIBQI/lib"

# realroot
REALROOT = "$$ARCH/realroot"
INCLUDEPATH += "$$REALROOT/include"
LIBS += -L"$$REALROOT/lib"

# qdecimal
QDECIMAL = "$$ARCH/qdecimal"
INCLUDEPATH += "$$QDECIMAL/include"
LIBS += -L"$$QDECIMAL/lib"

# optimization
CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -DCGAL_NDEBUG
    QMAKE_CXXFLAGS -= -D_DEBUG
    QMAKE_CXXFLAGS += -DNDEBUG
    QMAKE_CXXFLAGS -= -O2
    QMAKE_CXXFLAGS += -O3 -march=core2
}

# parallel
QMAKE_CXXFLAGS += -fopenmp
LIBS += -lgomp

# CS
INCLUDEPATH += "../cs"
LIBS += -L"../bin"

win32-g++:{
    QMAKE_CXXFLAGS += $$GXX_WARNING_FLAGS

    # boost needs that
    QMAKE_CXXFLAGS += -DBOOST_THREAD_USE_LIB

    # CGAL needs that
    QMAKE_CXXFLAGS += -frounding-math

    # boost
    BOOST = "$$ARCH/boost-1.49"
    INCLUDEPATH += "$$BOOST/include"
    LIBS += -L"$$BOOST/lib"

    # GMP
    GMP = "$$ARCH/gmp-5.0.5"
    INCLUDEPATH += "$$GMP/include"
    LIBS += -L"$$GMP/lib"

    # MPFR
    MPFR = "$$ARCH/mpfr-3.1.0"
    INCLUDEPATH += "$$MPFR/include"
    LIBS += -L"$$MPFR/lib"

    # MPFI
    MPFI = "$$ARCH/mpfi-1.5.1"
    INCLUDEPATH += "$$MPFI/include"
    LIBS += -L"$$MPFI/lib"

    # CGAL
    CGAL = "$$ARCH/CGAL-4.0"
    INCLUDEPATH += "$$CGAL/include"
    LIBS += -L"$$CGAL/lib"

    # APR
    APR = "$$ARCH/apr-1.4.6"
    INCLUDEPATH += "$$APR/include"
    LIBS += -L"$$APR/lib"

    # APRUTIL
    APRUTIL = "$$ARCH/apr-util-1.4.1"
    INCLUDEPATH += "$$APRUTIL/include"
    LIBS += -L"$$APRUTIL/lib"

    # LOG4CXX
    LOG4CXX = "$$ARCH/log4cxx-0.10.0"
    INCLUDEPATH += "$$LOG4CXX/include"
    LIBS += -L"$$LOG4CXX/lib"

    # GLEW
    GLEW = "$$ARCH/glew-1.7.0"
    INCLUDEPATH += "$$GLEW/include"
    LIBS += -L"$$GLEW/lib"

    # GLE
    GLE = "$$ARCH/gle-3.1.0"
    INCLUDEPATH += "$$GLE/include"
    LIBS += -L"$$GLE/lib"

    # VTK
    VTK = "$$ARCH/vtk-5.10.1"
    INCLUDEPATH += "$$VTK/include/vtk-5.10"
    LIBS += -L"$$VTK/lib/vtk-5.10"
}
linux*:{
    QMAKE_CXXFLAGS += $$GXX_WARNING_FLAGS

    # CGAL needs that
    QMAKE_CXXFLAGS += -frounding-math

    # VTK
    INCLUDEPATH += "/usr/include/vtk-5.8"
}

#
# libs
#
LIBS += -lqi
LIBS += -lLiDIA
LIBS += -lmpfi
LIBS += -lmpfr
LIBS += -lgmp
LIBS += -lCGAL
LIBS += -lrealroot
LIBS += -llog4cxx
LIBS += -lgle
LIBS += -lcs
LIBS += -lqdecimal
LIBS += -ldecnumber

win32-g++:{
    LIBS += -lboost_thread-mgw46-mt-1_49
    LIBS += -lapr-1
    LIBS += -laprutil-1
    LIBS += -lws2_32
    LIBS += -lexpat
    LIBS += -liconv
    LIBS += -lmswsock
    LIBS += -lglew32
    LIBS += -lopengl32
    LIBS += -lglu32
    LIBS += -lvtkGraphics
    LIBS += -lvtkFiltering
    LIBS += -lvtkImaging
    LIBS += -lvtkCommon
    LIBS += -lvtksys
    LIBS += -lgdi32
}
linux*:{
    LIBS += -lboost_thread
    LIBS += -lGLEW
    LIBS += -lGLU
    LIBS += -lvtkCommon
    LIBS += -lvtkGraphics
    LIBS += -lvtkFiltering
    LIBS += -lvtkImaging
}

#
# application icon
#
win32:{
    RC_FILE = windowsicon.rc
}

# libcs
INCLUDEPATH += ..

# project path
INCLUDEPATH += src

# sources
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/renderviewflycamera.cpp \
    src/renderviewarcballcamera.cpp \
    src/renderviewautocamera.cpp \
    src/renderview.cpp \
    src/vectorvalidator.cpp \
    src/planevalidator.cpp \
    src/numbervalidator.cpp \
    src/benchmarkdialog.cpp \
    src/triangleintersectiondialog.cpp \
    src/tritripredicatelist.cpp \
    src/sceneloader.cpp \
    src/multisplitter.cpp \
    src/variantpredicate.cpp \
    src/shader.cpp \
    src/variantpredicatedialog.cpp \
    src/predicateg.cpp \
    src/predicateh.cpp \
    src/predicates.cpp \
    src/mesh.cpp \
    src/logobackform.cpp \
    src/aboutdialog.cpp \
    src/clientform.cpp \
    src/sceneobject.cpp \
    src/sceneobjectdialog.cpp \
    src/colorwidget.cpp \
    src/rasterconfigurationspace.cpp \
    src/spin3.cpp \
    src/scopeddisablelighting.cpp \
    src/qlog4cxx.cpp \
    src/configurationobject.cpp \
    src/configurationobjectdialog.cpp \
    src/compressor.cpp \
    src/trianglelistmesh.cpp \
    src/ballmesh.cpp \
    src/gridmesh.cpp \
    src/polyconemesh.cpp \
    src/spheretreeloader.cpp \
    src/pointlistmesh.cpp \
    src/volumerenderertexture3d.cpp \
    src/volumerenderergaussiansplatter.cpp \
    src/material.cpp

# headers
HEADERS += \
    src/mainwindow.h \
    src/renderviewflycamera.h \
    src/renderviewcamera.h \
    src/renderviewarcballcamera.h \
    src/renderviewautocamera.h \
    src/configurationspace.h \
    src/renderview.h \
    src/vectorvalidator.h \
    src/planevalidator.h \
    src/numbervalidator.h \
    src/benchmarkdialog.h \
    src/triangleintersectiondialog.h \
    src/tritripredicatelist.h \
    src/sceneloader.h \
    src/multisplitter.h \
    src/variantpredicate.h \
    src/shader.h \
    src/variantpredicatedialog.h \
    src/predicateg.h \
    src/predicateh.h \
    src/predicates.h \
    src/mesh.h \
    src/logobackform.h \
    src/aboutdialog.h \
    src/clientform.h \
    src/sceneobject.h \
    src/kernel.h \
    src/sceneobjectdialog.h \
    src/colorwidget.h \    
    src/rasterconfigurationspace.h \
    src/spin3.h \
    src/scopeddisablelighting.h \
    src/ispoweroftwo.h \
    src/qlog4cxx.h \
    src/configurationobject.h \
    src/configurationobjectdialog.h \
    src/compressor.h \
    src/genericrouter.h \
    src/cellconfigurationspace.h \
    src/exactconfigurationspace.h \
    src/qdecimal.h \
    src/decimalscene.h \
    src/trianglelistmesh.h \
    src/ballmesh.h \
    src/gridmesh.h \
    src/polyconemesh.h \
    src/spheretreeloader.h \
    src/pointlistmesh.h \
    src/sampledroute.h \
    src/volumerenderertexture3d.h \
    src/volumerenderergaussiansplatter.h \
    src/material.h

# forms
FORMS += \
    src/mainwindow.ui \
    src/benchmarkdialog.ui \
    src/triangleintersectiondialog.ui \
    src/variantpredicatedialog.ui \
    src/predicateg.ui \
    src/predicates.ui \
    src/predicateh.ui \
    src/aboutdialog.ui \
    src/clientform.ui \
    src/sceneobjectdialog.ui \
    src/configurationobjectdialog.ui

RESOURCES += resources.qrc
