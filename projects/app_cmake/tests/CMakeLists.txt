# Enable testing
enable_testing()

# Try to find GTest first
find_package(GTest QUIET)

# Check if we have a complete GTest installation (including GMock)
if(GTest_FOUND AND TARGET GTest::gtest AND TARGET GTest::gtest_main)
    # Check if GMock is also available
    if(TARGET GTest::gmock AND TARGET GTest::gmock_main)
        set(USE_SYSTEM_GTEST TRUE)
        message(STATUS "Using complete system GTest with GMock")
    else()
        set(USE_SYSTEM_GTEST FALSE)
        message(STATUS "System GTest found but GMock missing - will download complete GTest+GMock")
    endif()
else()
    set(USE_SYSTEM_GTEST FALSE)
    message(STATUS "System GTest not found - will download from GitHub")
endif()

# Only download GTest if we're not using the system version
if(NOT USE_SYSTEM_GTEST)
    message(STATUS "Downloading GTest from GitHub...")

    # Include FetchContent module
    include(FetchContent)

    # Declare GTest dependency
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
            GIT_SHALLOW TRUE
            )

    # Make GTest available
    FetchContent_MakeAvailable(googletest)

    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Set downloaded libs instead of installed
    set(EXAMPLE_TEST_LIBS gtest gtest_main gmock gmock_main)
    message(STATUS "GTest downloaded and configured successfully")
else()
    set(EXAMPLE_TEST_LIBS GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
endif()

# Create test executable
add_executable(ExampleTests test_app.cpp)
target_include_directories(ExampleTests PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Link against GTest and our library
target_link_libraries(ExampleTests
                      PRIVATE
                      ${EXAMPLE_TEST_LIBS}
                      SubLib
                      )

# Add test to CTest
add_test(NAME ExampleTests COMMAND ExampleTests)

# Exclude tests from installation/packaging
set_target_properties(ExampleTests PROPERTIES
                      EXCLUDE_FROM_ALL TRUE
                      EXCLUDE_FROM_DEFAULT_BUILD TRUE
                      )