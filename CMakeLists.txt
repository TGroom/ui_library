cmake_minimum_required(VERSION 3.30.3)
project(ui_library VERSION 0.1.0.0 LANGUAGES C CXX)

# Set C++ standard and binary output directory.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Force static builds for external dependencies.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static libraries for dependencies" FORCE)

# Define version variables.
set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(VERSION_TWEAK ${PROJECT_VERSION_TWEAK})

# --- External Dependencies ---

# Enable FetchContent for external projects.
include(FetchContent)

# GLM: Header-only math library.
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        bf71a834948186f4097caa076cd2663c69a10e1e  # Specific version.
)
FetchContent_MakeAvailable(glm)

# GLFW: Window and context management.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable GLFW documentation build")
add_subdirectory(external/glfw-3.4)

# FreeType: Font rendering.
if(WIN32)
    # Build from source on Windows.
    add_subdirectory(external/freetype-2.10.0)
else()
    find_package(Freetype REQUIRED)
    if (NOT Freetype_FOUND)
        message(STATUS "FreeType not found. Building from source.")
        add_subdirectory(external/freetype-2.10.0)
    else()
        message(STATUS "Using system-installed FreeType.")
    endif()
endif()

# GLAD: OpenGL function loader.
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# --- Configure Generated Headers ---

# Generate a namespaced config header for the UI library.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui_library/Config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/ui_library/Config.h @ONLY
)

# --- UI Library Target ---

add_library(ui_library STATIC
    src/stb_image.cpp
    src/stb_image_write.cpp
    src/Shader.cpp
    src/VAO.cpp
    src/VBO.cpp
    src/EBO.cpp
    src/Utils.cpp
    src/Texture.cpp
    src/Text.cpp
    src/Button.cpp
    src/DropdownButton.cpp
    src/InputField.cpp
    src/Scrollbar.cpp
    src/WorkspaceContainer.cpp
    src/Application.cpp
)

target_include_directories(ui_library PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/include    # For generated config header.
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    external/glad/include
    external/glfw-3.4/include
    external/freetype-2.10.0/include
)

target_link_libraries(ui_library PUBLIC
    glad
    glfw
    freetype
    glm
)
