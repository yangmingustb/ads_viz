cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

find_package(GeographicLib)
include_directories(${GeographicLib_INCLUDE_DIRS})

add_subdirectory(pugixml)

file(GLOB LANELET_FILE
          Lanelet2/lanelet2_core/include/lanelet2_core/*.h
          Lanelet2/lanelet2_core/include/lanelet2_core/geometry/*.h
          Lanelet2/lanelet2_core/include/lanelet2_core/geometry/impl/*.h
          Lanelet2/lanelet2_core/include/lanelet2_core/primitives/*.h
          Lanelet2/lanelet2_core/include/lanelet2_core/utility/*.h
          Lanelet2/lanelet2_core/src/*.cpp

          Lanelet2/lanelet2_io/include/lanelet2_io/*.h
          Lanelet2/lanelet2_io/include/lanelet2_io/io_handlers/*.h
          Lanelet2/lanelet2_io/src/*.cpp

          Lanelet2/lanelet2_matching/include/lanelet2_matching/*.h
          Lanelet2/lanelet2_matching/src/*.cpp

          Lanelet2/lanelet2_projection/include/lanelet2_projection/*.h
          Lanelet2/lanelet2_projection/src/*.cpp

          Lanelet2/lanelet2_routing/include/lanelet2_routing/*.h
          Lanelet2/lanelet2_routing/include/lanelet2_routing/internal/*.h
          Lanelet2/lanelet2_routing/src/*.cpp

          Lanelet2/lanelet2_traffic_rules/include/lanelet2_traffic_rules/*.h
          Lanelet2/lanelet2_traffic_rules/src/*.cpp
        )
add_library(lanelet2 ${LANELET_FILE}
          )
target_include_directories(lanelet2
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Lanelet2/lanelet2_core/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Lanelet2/lanelet2_io/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Lanelet2/lanelet2_matching/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Lanelet2/lanelet2_projection/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Lanelet2/lanelet2_routing/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Lanelet2/lanelet2_traffic_rules/include/>
  )
target_link_libraries(lanelet2  PUBLIC ${Boost_LIBRARIES}
                                Eigen3::Eigen
                                pugixml
                                ${GeographicLib_LIBRARIES}
                                boost_system
                                boost_filesystem)

add_subdirectory(googletest)
# add_subdirectory(benchmark)


