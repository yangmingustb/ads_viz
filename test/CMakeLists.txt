cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Threads)


add_executable(hello_qt hello_qt.cc)
target_link_libraries(hello_qt Qt5::Widgets
                                # Threads::Threads
                                )

add_executable(hello_string hello_string.cc)
target_link_libraries(hello_string Qt5::Widgets)

# qt5_wrap_cpp(moc_files hello_widget.cc)
add_executable(hello_widget hello_widget.cc)
# set_target_properties(hello_widget PROPERTIES AUTOMOC TRUE)
target_link_libraries(hello_widget Qt5::Widgets)

add_executable(hello_ui hello_ui/hello_ui.cc)
target_link_libraries(hello_ui Qt5::Widgets)

add_executable(hello_signal hello_signal/hello_signal.cc
                            hello_signal/main.cc)
target_link_libraries(hello_signal Qt5::Widgets)

add_executable(hello_button 
                            hello_button/radio_button.cc)
target_link_libraries(hello_button Qt5::Widgets)

add_executable(hello_layout hello_layout.cc)
target_link_libraries(hello_layout Qt5::Widgets)

add_executable(hello_opengl opengl/opengl_window_test1.cc)
target_link_libraries(hello_opengl Qt5::Widgets Qt5::Gui Qt5::Core  ${OPENGL_LIBRARIES})

add_subdirectory(opengl)
add_subdirectory(hello_mainwindow)

# Generate the .h and .cxx files
file(GLOB proto_file ${CMAKE_CURRENT_SOURCE_DIR}/hello_proto/proto/test.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${proto_file})
add_executable(proto_test hello_proto/test_proto.cc  ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_test ${PROTOBUF_LIBRARIES})

add_executable(qt_osg_test test_osg/osg_qt_test.cc
                            )
target_link_libraries(qt_osg_test   Qt5::Widgets Qt5::Gui Qt5::Core
                                    ${OPENGL_LIBRARIES}
                                    Eigen3::Eigen 
                                    osg
                                    osgUtil
                                    osgDB
                                    osgText
                                    osgGA
                                    osgFX
                                    osgSim
                                    osgViewer
                                    osgManipulator
                                    ${OPENSCENEGRAPH_LIBRARIES}
                                    )

