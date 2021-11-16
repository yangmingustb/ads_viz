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

add_executable(lesson1  2d/mainwindow.cpp
                        2d/main.cpp)
target_link_libraries(lesson1   Qt5::Widgets Qt5::Gui Qt5::Core
                                ${OPENGL_LIBRARIES})

add_executable(lesson2  3d/mainwindow3d.cpp
                        3d/main.cpp)
target_link_libraries(lesson2 Qt5::Widgets Qt5::Gui Qt5::Core ${OPENGL_LIBRARIES})




