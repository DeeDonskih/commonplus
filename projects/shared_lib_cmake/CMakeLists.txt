cmake_minimum_required(VERSION 3.10)
project(MyLibraryProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# Создание библиотеки
add_library(MyLibrary SHARED
    src/functions.cpp
    src/MyClass.cpp
)

# Публичные и приватные include директории
target_include_directories(MyLibrary
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

# Установка библиотеки
install(TARGETS MyLibrary
    EXPORT MyLibraryTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MyLibrary
)

# Установка заголовков
install(DIRECTORY include/MyLibrary
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Экспорт targets
install(EXPORT MyLibraryTargets
    FILE MyLibraryTargets.cmake
    NAMESPACE MyLibrary::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyLibrary
)

# Конфигурация find_package
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MyLibraryConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MyLibraryConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyLibrary
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MyLibraryConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyLibrary
)

# Пример приложения
add_executable(TestApp main.cpp)
target_link_libraries(TestApp PRIVATE MyLibrary)
