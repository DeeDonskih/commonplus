cmake_minimum_required(VERSION 3.18)
project(ExampleApp VERSION 0.1.0 LANGUAGES C CXX)

# Project configuration
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set default build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Project options
option(ENABLE_MAXOPT "Enable maximum optimization with LTO" OFF)
option(ENABLE_PACK_EXAMPLE "Enable creating packages with cpack" ON)
option(ENABLE_TESTS_EXAMPLE "Enable building example test projects" OFF)
option(EMBEDDED_TARGET_OPTS "Disable Exceptions and RTTI(common for embedded projects). Example of conditional options" ON)

# Add custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(MapFileSupport)

# Target name configuration
set(MAIN_TARGET ${PROJECT_NAME})

# Find required dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(JPEG REQUIRED libjpeg)
find_package(Threads REQUIRED)

# Optional OpenCV dependency
find_package(OpenCV QUIET)

# Add subdirectory for SubLib
add_subdirectory(subLib)

# Source collection function
function(collect_sources out_var source_dir)
    file(GLOB_RECURSE source_files
            "${source_dir}/*.c"
            "${source_dir}/*.cpp"
    )
    file(GLOB_RECURSE header_files
            "${source_dir}/*.h"
            "${source_dir}/*.hpp"
    )
    set(${out_var} ${source_files} ${header_files} PARENT_SCOPE)
endfunction()

# Collect application sources
collect_sources(APP_SOURCES "${CMAKE_SOURCE_DIR}/src/sources")
message(STATUS "Collected sources: ${APP_SOURCES}")

# Create main executable
add_executable(${MAIN_TARGET}
        src/main.cpp
        ${APP_SOURCES}
)

# Configure include directories
target_include_directories(${MAIN_TARGET} PRIVATE
        src
        thirdparty
        ${JPEG_INCLUDE_DIRS}
)

# Link libraries using modern CMake approach
target_link_libraries(${MAIN_TARGET} PRIVATE
        SubLib
        ${JPEG_LINK_LIBRARIES}
        Threads::Threads
)

# Compiler-specific flags for exceptions and RTTI (optiopnal enabled/disabled)
target_compile_options(${MAIN_TARGET} PRIVATE $<$<BOOL:${EMBEDDED_TARGET_OPTS}>:-fno-rtti -fno-exceptions>)

# Handle OpenCV linking and definitions
if (OpenCV_FOUND)
    message(STATUS "OpenCV found: version ${OpenCV_VERSION}")
    target_link_libraries(${MAIN_TARGET} PRIVATE ${OpenCV_LIBS})
else ()
    message(WARNING "OpenCV not found, building without OpenCV support")
    target_compile_definitions(${MAIN_TARGET} PRIVATE MA_NO_OPENCV)
endif ()

# Platform-specific configuration
if (APPLE)
    set_target_properties(${MAIN_TARGET} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist.in"
            MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    )
endif ()

# Link-time optimization (LTO) support
if (ENABLE_MAXOPT)
    message(STATUS "Maximum optimization requested")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

    if (ipo_supported)
        message(STATUS "IPO/LTO enabled")
        set_target_properties(${MAIN_TARGET} PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION TRUE
        )
        set_target_properties(SubLib PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION TRUE
        )
    else ()
        message(WARNING "IPO/LTO not supported: ${ipo_error}")
    endif ()
endif ()

# Enable map file generation
enable_mapfile(${MAIN_TARGET})
enable_mapfile(SubLib)

# Standard installation paths
include(GNUInstallDirs)

# Installation configuration
install(TARGETS ${MAIN_TARGET}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION .
)

install(TARGETS SubLib
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# This enables packaging with CPack
# `cpack -G DEB` - for Debian package
# `cpack -G RPM` - for RPM
# `cpack -G "DEB;RPM;TGZ"` - for all
if (ENABLE_PACK_EXAMPLE)
    # CPack configuration
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_CONTACT "developer@example.com")
    set(CPACK_PACKAGE_VENDOR "ExampleDeveloper")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Example CMake application")
    set(CPACK_PACKAGE_DESCRIPTION "A comprehensive example of a CMake-based C++ application
with external dependencies and shared libraries.")

    # Platform-specific packaging
    if (APPLE)
        # Apple .dmg image
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
        set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/Info.plist.in")
        set(CPACK_BUNDLE_STARTUP_COMMAND "${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME}")
    elseif (UNIX)
        # Linux packaging
        set(CPACK_GENERATOR "DEB;RPM;STGZ;TGZ")
        # DEB package configuration
        set(CPACK_BINARY_DEB ON)
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://example.com")
        # Dependencies for DEB package
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libjpeg-dev, libpthread-stubs0-dev")
        if (OpenCV_FOUND)
            set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libopencv-dev")
        endif ()

        # RPM package configuration
        set(CPACK_RPM_PACKAGE_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
        set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
        set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
        set(CPACK_RPM_PACKAGE_URL "https://example.com")

        # Dependencies for RPM package
        set(CPACK_RPM_PACKAGE_REQUIRES "libjpeg-devel, glibc-devel")
        if (OpenCV_FOUND)
            set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, opencv-devel")
        endif ()
    endif ()

    # Set package file names
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    include(CPack)
endif ()

# Testing configuration
if (ENABLE_TESTS_EXAMPLE)
    add_subdirectory(tests)
endif ()

# Display configuration summary
message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "ARCH: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler C:${CMAKE_C_COMPILER_ID} C++:${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "OpenCV: ${OpenCV_FOUND}")
message(STATUS "Max optimization: ${ENABLE_MAXOPT}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Binary dir: ${CMAKE_INSTALL_FULL_BINDIR}")
message(STATUS "Library dir: ${CMAKE_INSTALL_FULL_LIBDIR}")
if (ENABLE_PACK_EXAMPLE AND APPLE)
    message(STATUS "Package type: DMG image")
elseif (ENABLE_PACK_EXAMPLE AND UNIX)
    message(STATUS "Package types: DEB, RPM, TGZ")
endif ()
message(STATUS "===================================")