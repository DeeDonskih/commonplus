# Modern C++/C project template with packaging, testing, and optional features.
cmake_minimum_required(VERSION 3.18)

project(ExampleApp
        VERSION 0.1.0
        LANGUAGES C CXX
        DESCRIPTION "Example C++ project"
        HOMEPAGE_URL "https://example.com"
        )

# ==== Options ====
option(ENABLE_PACKAGING "Enable package creation with CPack." ON)
option(ENABLE_TESTS "Enable building of test projects (add_subdirectory(tests))." ON)
option(EMBEDDED_TARGET_OPTS "Disable exceptions and RTTI (common for embedded projects)." OFF)
option(ENABLE_MAXOPT "Enable maximum optimization with LTO." OFF)

# ==== Build Type ====
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")
endif()

# ==== Language Standards ====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Disable GNU extensions.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ==== Custom Modules Path ====
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(MapFileSupport)

# ==== Main Target Name ====
set(MAIN_TARGET "${PROJECT_NAME}")

# ==== Dependencies ====
find_package(PkgConfig REQUIRED)
pkg_check_modules(JPEG REQUIRED libjpeg)
find_package(Threads REQUIRED)
find_package(OpenCV QUIET) # Optional

# ==== Subprojects ====
add_subdirectory(subLib)

# ==== Source Collection (Anti-pattern, see note below) ====
# Note: file(GLOB ...) is not recommended for production because new source files will not trigger a CMake reconfiguration automatically.
function(collect_sources out_var source_dir)
    file(GLOB_RECURSE source_files CONFIGURE_DEPENDS
         "${source_dir}/*.c"
         "${source_dir}/*.cpp"
         )
    file(GLOB_RECURSE header_files CONFIGURE_DEPENDS
         "${source_dir}/*.h"
         "${source_dir}/*.hpp"
         )
    set(${out_var} ${source_files} ${header_files} PARENT_SCOPE)
endfunction()
collect_sources(APP_SOURCES "${CMAKE_SOURCE_DIR}/src/sources") # This is an anti-pattern; use explicit lists for real projects.

# ==== Executable ====
add_executable(${MAIN_TARGET}
               src/main.cpp
               ${APP_SOURCES}
               )

# ==== Include Directories ====
target_include_directories(${MAIN_TARGET} PRIVATE
                           src
                           thirdparty
                           "${JPEG_INCLUDE_DIRS}"
                           )

# ==== Link Libraries ====
target_link_libraries(${MAIN_TARGET} PRIVATE
                      SubLib
                      "${JPEG_LINK_LIBRARIES}"
                      Threads::Threads
                      )

# ==== Compiler Flags (for Embedded, disables exceptions/RTTI) ====
target_compile_options(${MAIN_TARGET} PRIVATE
                       $<$<BOOL:${EMBEDDED_TARGET_OPTS}>:-fno-rtti -fno-exceptions>
                       )

# ==== OpenCV Integration ====
if(OpenCV_FOUND)
    message(STATUS "OpenCV found: version ${OpenCV_VERSION}")
    target_link_libraries(${MAIN_TARGET} PRIVATE ${OpenCV_LIBS})
else()
    message(WARNING "OpenCV not found; building without OpenCV support.")
    target_compile_definitions(${MAIN_TARGET} PRIVATE EXAMPLE_NO_OPENCV_DEF)
endif()

# ==== Platform-Specific Configuration ====
if(APPLE)
    set_target_properties(${MAIN_TARGET} PROPERTIES
                          MACOSX_BUNDLE TRUE
                          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist.in"
                          MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
                          MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
                          )
endif()

# ==== Link-Time Optimization (LTO/IPO) ====
if(ENABLE_MAXOPT)
    message(STATUS "Maximum optimization requested.")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

    if(ipo_supported)
        message(STATUS "IPO/LTO enabled.")
        set_target_properties(${MAIN_TARGET} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_target_properties(SubLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO/LTO not supported: ${ipo_error}")
    endif()
endif()

# ==== Map File Generation (requires custom module) ====
enable_mapfile(${MAIN_TARGET})
enable_mapfile(SubLib)

# ==== Install Paths ====
# Use standard GNU directory structure
include(GNUInstallDirs)

install(TARGETS ${MAIN_TARGET}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        BUNDLE DESTINATION "."
        )
install(TARGETS SubLib
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        )

# ==== Packaging ====
if(ENABLE_PACKAGING)
    # Common package information
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_CONTACT "developer@example.com")
    set(CPACK_PACKAGE_VENDOR "ExampleDeveloper")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Example CMake application.")
    set(CPACK_PACKAGE_DESCRIPTION "A comprehensive example of a CMake-based C++ application with external dependencies and shared libraries.")

    if(APPLE)
        # macOS DMG package
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
        set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/Info.plist.in")
        set(CPACK_BUNDLE_STARTUP_COMMAND "${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME}")
    elseif(UNIX)
        # Linux packages: DEB, RPM, and archives
        set(CPACK_GENERATOR "DEB;RPM;STGZ;TGZ")

        # Debian package configuration.
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://example.com")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libjpeg-dev, libpthread-stubs0-dev")
        if(OpenCV_FOUND)
            set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libopencv-dev")
        endif()

        # RPM package configuration.
        set(CPACK_RPM_PACKAGE_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
        set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
        set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
        set(CPACK_RPM_PACKAGE_URL "https://example.com")
        set(CPACK_RPM_PACKAGE_REQUIRES "libjpeg-devel, glibc-devel")
        if(OpenCV_FOUND)
            set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, opencv-devel")
        endif()
    endif()

    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    include(CPack)
endif()

# ==== Testing ====
if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()

# ==== Build Configuration Summary ====
message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Project:            ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "System:             ${CMAKE_SYSTEM}")
message(STATUS "ARCH:               ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler:           C: ${CMAKE_C_COMPILER_ID} C++: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard:       ${CMAKE_CXX_STANDARD}")
message(STATUS "C Standard:         ${CMAKE_C_STANDARD}")
message(STATUS "OpenCV:             ${OpenCV_FOUND}")
message(STATUS "Max optimization:   ${ENABLE_MAXOPT}")
message(STATUS "Install prefix:     ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Binary dir:         ${CMAKE_INSTALL_FULL_BINDIR}")
message(STATUS "Library dir:        ${CMAKE_INSTALL_FULL_LIBDIR}")
if(ENABLE_PACKAGING AND APPLE)
    message(STATUS "Package type:       DMG image")
elseif(ENABLE_PACKAGING AND UNIX)
    message(STATUS "Package types:      DEB, RPM, STGZ, TGZ")
endif()
message(STATUS "===================================")