cmake_minimum_required(VERSION 3.16)
project(CatMarkSubdiv VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS OpenGL OpenGLWidgets Widgets)

qt_add_executable(CatMarkSubdiv WIN32 MACOSX_BUNDLE
    initialization/meshinitializer.cpp initialization/meshinitializer.h
    initialization/objfile.cpp initialization/objfile.h
    main.cpp
    mainview.cpp mainview.h
    mainwindow.cpp mainwindow.h mainwindow.ui
    mesh/face.cpp mesh/face.h
    mesh/halfedge.cpp mesh/halfedge.h
    mesh/mesh.cpp mesh/mesh.h
    mesh/vertex.cpp mesh/vertex.h
    renderers/meshrenderer.cpp renderers/meshrenderer.h
    renderers/tessrenderer.cpp renderers/tessrenderer.h
    renderers/renderer.cpp renderers/renderer.h
    settings.h
    shadertypes.h
    subdivision/subdivider.cpp
    subdivision/catmullclarksubdivider.cpp subdivision/catmullclarksubdivider.h
    subdivision/subdivider.h
    util/util.h util/util.cpp
    resources.qrc
)
target_link_libraries(CatMarkSubdiv PRIVATE
    Qt::Core
    Qt::Gui
)

if((QT_VERSION_MAJOR GREATER 5))
    target_link_libraries(CatMarkSubdiv PRIVATE
        Qt::OpenGL
        Qt::OpenGLWidgets
        Qt::Widgets
    )
endif()

install(TARGETS CatMarkSubdiv
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
