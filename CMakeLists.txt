cmake_minimum_required(VERSION 2.8.9)

project(QuickCollider)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)

find_package(Qt5Quick REQUIRED)

# For moc to work properly:
include_directories( ${Qt5Qml_INCLUDE_DIRS} )

set( moc_hdr
    osc/osc_server.hpp
    osc/osc_client.hpp
    osc/osc_dispatcher.hpp
    osc/qml_osc_interface.hpp
    gui/model/slider_model.hpp
    gui/model/multi_slider_model.hpp
    gui/model/graph_model.hpp
    gui/widgets/graph_plotter.hpp
)

set( src
    app/main.cpp
    osc/osc_server.cpp
    osc/osc_client.cpp
    osc/osc_dispatcher.cpp
    osc/qml_osc_interface.cpp
    gui/model/graph_model.cpp
    gui/widgets/graph_plotter.cpp
)

qt5_wrap_cpp( moc_src ${moc_hdr} )

add_executable(quickcollider ${src} ${moc_src})

qt5_use_modules(quickcollider Quick)
target_link_libraries(quickcollider lo)
