# Minimum CMake version required
cmake_minimum_required(VERSION 3.5)

set(PKG_CONFIG_EXECUTABLE F:/Development/vcpkg/vcpkg/packages/pkgconf_x64-windows/tools/pkgconf/pkgconf.exe)
# Define the project name
project(gtk_calculator LANGUAGES C)
# --- Build Configuration Flags ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET GLOBAL gtk4)
pkg_check_modules(LIBADWAITA REQUIRED IMPORTED_TARGET GLOBAL libadwaita-1)

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


# --- Target Definition ---2

# Add the executable target
add_executable(gtk_calculator src/main.c src/Application.c src/Calculator.c app_manifest.rc)

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
