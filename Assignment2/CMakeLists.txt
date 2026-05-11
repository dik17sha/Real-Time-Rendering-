cmake_minimum_required(VERSION 3.10)
project(RealTimeRendering CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# ============ ImGui Sources ============
# Point this to where you placed the ImGui files
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/libs/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
)

# ============ Collect Sources ============
set(SOURCES
    src/main.cpp
    src/stb.cpp
    libs/src/glad.c
    ${IMGUI_SOURCES}
)

# ============ Create Executable ============
add_executable(${PROJECT_NAME} ${SOURCES})

# ============ Include Directories ============
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/headers"
    "${CMAKE_SOURCE_DIR}/libs/include"
    "${CMAKE_SOURCE_DIR}/src"
    "${IMGUI_DIR}"
)

# ============ Find OpenGL ============
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# ============ Link Assimp ============
set(ASSIMP_LIB_PATH "${CMAKE_SOURCE_DIR}/libs/lib")
if(EXISTS "${ASSIMP_LIB_PATH}/libassimp.a")
    message(STATUS "Using libassimp.a from ${ASSIMP_LIB_PATH}")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${ASSIMP_LIB_PATH}/libassimp.a")
else()
    message(FATAL_ERROR "Assimp library not found in ${ASSIMP_LIB_PATH}")
endif()

# ============ Link zlib (required by Assimp) ============
find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)

# ============ Link GLFW ============
set(GLFW_LIB_PATH "${CMAKE_SOURCE_DIR}/libs/lib")

# Try static library first, then dynamic
if(EXISTS "${GLFW_LIB_PATH}/libglfw3.a")
    message(STATUS "Using libglfw3.a from ${GLFW_LIB_PATH}")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${GLFW_LIB_PATH}/libglfw3.a")
elseif(EXISTS "${GLFW_LIB_PATH}/libglfw.3.dylib")
    message(STATUS "Using libglfw.3.dylib from ${GLFW_LIB_PATH}")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${GLFW_LIB_PATH}/libglfw.3.dylib")
else()
    message(FATAL_ERROR "GLFW library not found in ${GLFW_LIB_PATH}. Expected libglfw3.a or libglfw.3.dylib")
endif()

# ============ macOS Frameworks ============
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# ============ Output ============
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# ============ Copy Assets & Shaders ============
# Copy shaders and assets to project root so they're found at runtime
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/shaders"
        "${CMAKE_SOURCE_DIR}/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_SOURCE_DIR}/assets"
    COMMENT "Copying shaders and assets to project root"
)

message(STATUS "✓ Project configured: ${PROJECT_NAME}")
message(STATUS "  Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "  Include paths: src/headers, libs/include, src")
