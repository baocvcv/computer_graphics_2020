CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(FINAL CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


ADD_SUBDIRECTORY(deps/vecmath)

SET(FINAL_SOURCES
        src/camera.cpp
        src/image.cpp
        src/main.cpp
        src/mesh.cpp
        src/scene_parser.cpp
        )

SET(FINAL_INCLUDES
        include/camera.hpp
        include/curve.hpp
        include/group.hpp
        include/helpers.hpp
        include/image.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/revsurface.hpp
        include/scene_parser.hpp
        include/json.hpp
        include/args.hxx
        )

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${FINAL_SOURCES} ${FINAL_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
