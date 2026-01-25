# Minimal CMake for Real-Time Rendering Assignment1
cmake_minimum_required(VERSION 3.10)

project(RTR_Assignment1 LANGUAGES C CXX)

# Use a reasonably modern standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Where to look for headers bundled with the repo
include_directories("${CMAKE_SOURCE_DIR}/libs/include"
					"${CMAKE_SOURCE_DIR}/headers")

# Collect sources
file(GLOB PROJECT_SOURCES
	"${CMAKE_SOURCE_DIR}/Source code/*.cpp"
)

# Add GLAD source
list(APPEND PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/libs/src/glad.c")

# Add executable
add_executable(RTR_Assignment1 ${PROJECT_SOURCES})

# Ensure GLAD C sources are properly compiled
set_source_files_properties(
    "${CMAKE_SOURCE_DIR}/libs/src/glad.c"
    PROPERTIES
    LANGUAGE C
    COMPILE_FLAGS "-Wno-sign-conversion -Wno-unused-parameter"
)

# On macOS we need to link the OpenGL framework
if(APPLE)
	find_library(OpenGL_FRAMEWORK OpenGL)
	if(OpenGL_FRAMEWORK)
		target_link_libraries(RTR_Assignment1 PRIVATE ${OpenGL_FRAMEWORK})
	else()
		find_package(OpenGL REQUIRED)
		target_link_libraries(RTR_Assignment1 PRIVATE ${OPENGL_gl_LIBRARY})
	endif()
else()
	find_package(OpenGL REQUIRED)
	target_link_libraries(RTR_Assignment1 PRIVATE ${OPENGL_gl_LIBRARY})
endif()

# Use GLFW from our libs directory
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/include")
set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/libs/lib/libglfw.3.dylib")

message(STATUS "Using bundled GLFW: ${GLFW_LIBRARY}")
target_link_libraries(RTR_Assignment1 PRIVATE ${GLFW_LIBRARY})

# Link Assimp static library
set(ASSIMP_LIBRARY "${CMAKE_SOURCE_DIR}/libs/lib/libassimp.a")
message(STATUS "Using bundled Assimp: ${ASSIMP_LIBRARY}")
target_link_libraries(RTR_Assignment1 PRIVATE 
	${ASSIMP_LIBRARY}
	z
)

# Add some useful compile flags for macOS
if(APPLE)
	target_compile_definitions(RTR_Assignment1 PRIVATE GL_SILENCE_DEPRECATION)
	# Cocoa/IOKit/CoreVideo frameworks are required for OpenGL windowing on macOS
	target_link_libraries(RTR_Assignment1 PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

# Set RPATH for bundled libraries on macOS
set_target_properties(RTR_Assignment1 PROPERTIES
	BUILD_RPATH "@executable_path/../libs/lib"
	INSTALL_RPATH "@executable_path/../libs/lib"
	BUILD_WITH_INSTALL_RPATH TRUE
)

# Enable GLM extensions
target_compile_definitions(RTR_Assignment1 PRIVATE GLM_ENABLE_EXPERIMENTAL)

# GLM is header-only and already provided under libs/include/glm, nothing to link.

# Include folders for target
target_include_directories(RTR_Assignment1 PRIVATE
	"${CMAKE_SOURCE_DIR}/libs/include"
    "${CMAKE_SOURCE_DIR}/libs/src"
	"${CMAKE_SOURCE_DIR}/headers"
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(RTR_Assignment1 PRIVATE 
        -Wall
        -Wextra
        -Wconversion
    )
elseif(MSVC)
    target_compile_options(RTR_Assignment1 PRIVATE /W4)
endif()

# Copy shader files into the build directory
file(GLOB PROJECT_SHADERS
	"${CMAKE_SOURCE_DIR}/shaders/*"
)
if(PROJECT_SHADERS)
	message(STATUS "Installing shader files into build dir")
	file(COPY ${PROJECT_SHADERS} DESTINATION ${CMAKE_BINARY_DIR})
endif()

