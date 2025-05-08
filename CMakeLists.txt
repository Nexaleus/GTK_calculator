# Minimum CMake version required
cmake_minimum_required(VERSION 3.5)

set(PKG_CONFIG_EXECUTABLE F:/Development/vcpkg/vcpkg/packages/pkgconf_x64-windows/tools/pkgconf/pkgconf.exe)
# Define the project name
project(gtk_calculator LANGUAGES C)

# --- Build Configuration Flags ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET GLOBAL gtk4)
pkg_check_modules(LIBADWAITA REQUIRED IMPORTED_TARGET GLOBAL libadwaita-1)

# Find the glib-compile-resources utility
find_program(GLIB_COMPILE_RESOURCES_EXECUTABLE glib-compile-resources)
if(NOT GLIB_COMPILE_RESOURCES_EXECUTABLE)
    message(FATAL_ERROR "glib-compile-resources not found. Please install GLib development tools.")
endif()


# Common flags for all builds (optional, add if needed)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4") # Example: Enable more warnings

# Debug specific flags (Debugging symbols, disable optimization)
# /Zi : Program Database for Edit &amp; Continue
# /Od : Disable optimization
# /MDd: Link with Debug Multithreaded DLL runtime
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi /Od /MDd")

# Release specific flags (Optimization, define NDEBUG to disable asserts)
# /O2 : Optimize for speed
# /DNDEBUG : Define NDEBUG macro
# /MD : Link with Multithreaded DLL runtime
# /GL : Enable Whole Program Optimization (optional, can improve performance but increase link time)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /DNDEBUG /MD")
# set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG") # Link-Time Code Generation (pairs with /GL)

# --- GResource Compilation ---
# Define the C identifier prefix for your resource bundle
set(RESOURCE_C_IDENTIFIER "main_application_resources")

# Define the path to your gresource XML file (relative to CMAKE_CURRENT_SOURCE_DIR)
set(GRESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.gresource.xml")

# Define the generated C source file name
set(GRESOURCE_GENERATED_C "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_C_IDENTIFIER}.c")
# Define the generated H header file name
set(GRESOURCE_GENERATED_H "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_C_IDENTIFIER}.h")


# Command to compile the GResource XML into a C source file
add_custom_command(
    OUTPUT ${GRESOURCE_GENERATED_C}
    COMMAND ${GLIB_COMPILE_RESOURCES_EXECUTABLE}
        --target=${GRESOURCE_GENERATED_C}
        --generate-source
        --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/resources # Specify the source directory for files listed in XML
        ${GRESOURCE_XML}
    DEPENDS
        ${GRESOURCE_XML}
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/main_ui_layout.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/style.css
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app-icon.png
    COMMENT "Compiling GResource XML to C source: ${GRESOURCE_XML}"
)

# Command to compile the GResource XML into an H header file
add_custom_command(
    OUTPUT ${GRESOURCE_GENERATED_H}
    COMMAND ${GLIB_COMPILE_RESOURCES_EXECUTABLE}
        --target=${GRESOURCE_GENERATED_H}
        --generate-header
        --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/resources # Specify the source directory for files listed in XML
        ${GRESOURCE_XML}
    DEPENDS
        ${GRESOURCE_XML}
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/main_ui_layout.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/style.css
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app-icon.png
    COMMENT "Compiling GResource XML to H header: ${GRESOURCE_XML}"
)

# --- Target Definition ---
# Add the executable target
add_executable(gtk_calculator src/main.c src/Application.c src/Calculator.c app_manifest.rc)

# Add the generated GResource C file to the executable sources !!!!!!
target_sources(gtk_calculator PRIVATE ${GRESOURCE_GENERATED_C} ${GRESOURCE_GENERATED_H})

# Add the directory containing the generated header to the include directories
target_include_directories(gtk_calculator PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Link GTK4 to the executable
#target_link_libraries(gtk_calculator PUBLIC PkgConfig::GTK4)
target_link_libraries(gtk_calculator PUBLIC PkgConfig::GTK4 PkgConfig::LIBADWAITA)

# Hide console window for Release builds on Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_BUILD_TYPE STREQUAL "Release")
  set_target_properties(gtk_calculator PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Optional: Specify C standard if needed (e.g., C11 or C17)
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED True)
# target_compile_features(calculator PRIVATE c_std_11)

# --- Output Directories (Optional but Recommended) ---
# Keep build artifacts organized
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Build Type Message ---
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type: Debug Release RelWithDebInfo MinSizeRel" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Debug.")
else()
  message(STATUS "CMAKE_BUILD_TYPE is set to ${CMAKE_BUILD_TYPE}.")
endif()

message(STATUS "Compiler Flags (Debug): ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Compiler Flags (Release): ${CMAKE_C_FLAGS_RELEASE}")
