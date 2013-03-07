cmake_minimum_required(VERSION 2.8.9)

project(QuickCollider)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)

find_package(Qt5Quick REQUIRED)

find_package(Dependencies REQUIRED)

if(NOT SC_SOURCE_DIR)
    message( FATAL_ERROR
             "SuperCollider source directory has not been provided.\n"
             "Please set the SC_SOURCE_DIR variable!" )
endif()

# For moc to work properly:
include_directories( ${Qt5Qml_INCLUDE_DIRS} )

include_directories( ${SC_SOURCE_DIR} )
include_directories( "${SC_SOURCE_DIR}/external_libraries/boost" )
include_directories( "${SC_SOURCE_DIR}/external_libraries/TLSF-2.4.6/src" )

include_directories( ${LO_INCLUDE_DIR} )
include_directories( ${SNDFILE_INCLUDE_DIR} )

set( moc_hdr
    osc/osc_server.hpp
    osc/osc_client.hpp
    osc/osc_dispatcher.hpp
    osc/qml_osc_interface.hpp
    gui/model/slider_model.hpp
    gui/model/multi_slider_model.hpp
    gui/model/graph_model.hpp
    gui/widgets/graph_plotter.hpp
    gui/widgets/oscilloscope.hpp
    gui/widgets/sf_view.hpp
    gui/utility/array_layout.hpp
    gui/utility/mapping.hpp
)

set( src
    app/main.cpp
    osc/osc_server.cpp
    osc/osc_client.cpp
    osc/osc_dispatcher.cpp
    osc/qml_osc_interface.cpp
    gui/model/graph_model.cpp
    gui/widgets/graph_plotter.cpp
    gui/widgets/oscilloscope.cpp
    gui/widgets/sf_view.cpp
    gui/widgets/sf_cache_stream.cpp
    gui/widgets/sf_file_stream.cpp
)

qt5_wrap_cpp( moc_src ${moc_hdr} )

add_executable(quickcollider ${src} ${moc_src})

qt5_use_modules(quickcollider Quick)
target_link_libraries(quickcollider ${LO_LIBRARY} ${SNDFILE_LIBRARY})
