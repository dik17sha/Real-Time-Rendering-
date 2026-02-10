cmake_minimum_required(VERSION 3.10)
project(RealTimeRendering LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
set(SRC_FILES
    src/main.cpp
    src/mesh.cpp
    src/stb.cpp
    libs/src/glad.c
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_impl_glfw.cpp
    libs/imgui/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Includes (project headers and bundled third-party headers)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/headers
    ${PROJECT_SOURCE_DIR}/libs/include
    ${PROJECT_SOURCE_DIR}/libs/imgui
)

# OpenGL
find_package(OpenGL REQUIRED)
if(TARGET OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
elseif(OPENGL_gl_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_gl_LIBRARY})
endif()

# zlib (required by Assimp for compressed model support)
find_library(ZLIB_LIBRARY NAMES z zlib)
if(ZLIB_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_LIBRARY})
endif()

# macOS system frameworks used by GLFW when linking statically
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    if(COCOA_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${COCOA_LIBRARY})
    endif()
    if(IOKIT_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${IOKIT_LIBRARY})
    endif()
    if(COREVIDEO_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${COREVIDEO_LIBRARY})
    endif()
endif()

# Link bundled prebuilt libraries (if present)
set(BUNDLED_LIB_DIR ${PROJECT_SOURCE_DIR}/libs/lib)
if(EXISTS "${BUNDLED_LIB_DIR}/libglfw3.a")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${BUNDLED_LIB_DIR}/libglfw3.a")
elseif(EXISTS "${BUNDLED_LIB_DIR}/libglfw.3.dylib")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${BUNDLED_LIB_DIR}/libglfw.3.dylib")
endif()

if(EXISTS "${BUNDLED_LIB_DIR}/libassimp.a")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${BUNDLED_LIB_DIR}/libassimp.a")
endif()

# Link dl on Linux (not on macOS)
if(UNIX AND NOT APPLE)
    find_library(DL_LIBRARY dl)
    if(DL_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${DL_LIBRARY})
    endif()
endif()

# Output folder for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build
)

# Helpful message
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Source files: ${SRC_FILES}")