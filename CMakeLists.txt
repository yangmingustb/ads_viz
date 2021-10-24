cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENSCENEGRAPH_INCLUDE_DIRS}
    )
include(GNUInstallDirs)

# find_package(OpenSceneGraph REQUIRED COMPONENTS
    # osgUtil osgDB osgText osgGA osgFX osgSim osgViewer osg osgManipulator CONFIG)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Eigen3 3 REQUIRED)
find_package(Threads)


add_subdirectory(test)
add_subdirectory(src)
add_subdirectory(third_party)

install(DIRECTORY include/ DESTINATION include/)

