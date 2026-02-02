cmake_minimum_required(VERSION 3.10)
project(RealTimeRendering CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# ============ Collect Sources ============
set(SOURCES
    src/main.cpp
    libs/src/glad.c
)

# ============ Create Executable ============
add_executable(${PROJECT_NAME} ${SOURCES})

# ============ Include Directories ============
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/headers"
    "${CMAKE_SOURCE_DIR}/libs/include"
    "${CMAKE_SOURCE_DIR}/src"
)

# ============ Find OpenGL ============
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

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
