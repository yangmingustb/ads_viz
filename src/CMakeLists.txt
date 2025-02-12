cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(
    include
    )


add_library(viz_lib osg_utils.cc
                    viz.cc
                    map.cc
                    osg_camera_control.cc
                    )
target_include_directories(viz_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(viz_lib PUBLIC Qt5::Widgets
                                Qt5::Gui
                                Qt5::Core
                                ${OPENGL_LIBRARIES}
                                Eigen3::Eigen
                                ${OPENSCENEGRAPH_LIBRARIES}
                                
                                lanelet2
                                boost_system
                                boost_filesystem
                                boost_serialization)

add_executable(main_cat  main.cc)
target_link_libraries(main_cat  viz_lib )





