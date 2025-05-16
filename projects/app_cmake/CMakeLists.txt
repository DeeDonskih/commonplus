cmake_minimum_required(VERSION 3.14)
project(MyApp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(MAXOPT "Enable maximum optimization with LTO" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(MapFileSupport)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JPEG REQUIRED libjpeg)
pkg_check_modules(PTHREAD REQUIRED pthread)

add_subdirectory(sub)

add_executable(MyApp src/main.cpp)
target_include_directories(MyApp PRIVATE src thirdparty ${JPEG_INCLUDE_DIRS})
target_link_libraries(MyApp PRIVATE SubLib ${JPEG_LIBRARIES} ${PTHREAD_LIBRARIES})

if(APPLE)
    set_target_properties(MyApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist.in"
    )
endif()

if(MAXOPT)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result)
    if(result)
        set_target_properties(MyApp PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_target_properties(SubLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

enable_mapfile(MyApp)
enable_mapfile(SubLib)

install(TARGETS MyApp
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

install(TARGETS SubLib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY thirdparty/ DESTINATION include/thirdparty FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/ DESTINATION include/app FILES_MATCHING PATTERN "*.h")
install(DIRECTORY sub/ DESTINATION include/sub FILES_MATCHING PATTERN "*.h")

include(CPack)
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "MyApp")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "your@email.com")
set(CPACK_PACKAGE_VENDOR "MyCompany")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_BUNDLE_NAME "MyApp")
    set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/Info.plist.in")
    set(CPACK_BUNDLE_STARTUP_COMMAND "MyApp.app/Contents/MacOS/MyApp")
endif()

enable_testing()
add_subdirectory(tests)