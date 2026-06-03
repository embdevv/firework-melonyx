cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

project(firework-melonyx C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE PATH "Third-party libs directory" FORCE)

# ── GLFW ──────────────────────────────────────────────────────────────────────
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ── Glad ──────────────────────────────────────────────────────────────────────
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.6
    SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_46 STATIC
    REPRODUCIBLE
    LOADER
    API gl:core=4.6
)

# ── GLM ───────────────────────────────────────────────────────────────────────
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# ── OpenGL ────────────────────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)

# ── Sources ───────────────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(firework-melonyx ${SOURCES})

# ── Includes ──────────────────────────────────────────────────────────────────
target_include_directories(firework-melonyx PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ── Link ──────────────────────────────────────────────────────────────────────
target_link_libraries(firework-melonyx PRIVATE
    glfw
    OpenGL::GL
    glad_gl_core_46
    glm::glm
)

# Windows
if(WIN32)
    target_link_libraries(firework-melonyx PRIVATE opengl32)
endif()

# Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(firework-melonyx PRIVATE dl pthread)
endif()

# ── Copy shaders next to the executable ───────────────────────────────────────
add_custom_command(TARGET firework-melonyx POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:firework-melonyx>/shaders"
    COMMENT "Copying shaders to output directory"
)