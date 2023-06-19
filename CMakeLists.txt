cmake_minimum_required(VERSION 3.5)

project(elevator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external)

message("Add external includes directories: " ${EXTERNAL_INCLUDES})
include_directories(${EXTERNAL_INCLUDES})

message("Add external libs directories: " ${EXTERNAL_LINK_DIRS})
link_directories(${EXTERNAL_LINK_DIRS})

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp

    src/elevator.cpp
    src/elevator.hpp

    src/window.cpp
    src/window.hpp
)

message("Link external libs: " ${EXTERNAL_LIBS})
target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        opengl32
        ${EXTERNAL_LIBS}
)

