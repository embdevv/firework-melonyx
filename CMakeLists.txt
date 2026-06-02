cmake_minimum_required(VERSION 3.15)
project(GDPHYSX_Phase1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(GDPHYSX_Phase1 ${SOURCES})

target_include_directories(GDPHYSX_Phase1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(GDPHYSX_Phase1 PRIVATE 
    OpenGL::OpenGL
    GLFW::glfw
    GLEW::GLEW
    glm::glm
)
