find_package(GTest REQUIRED)

add_executable(MyAppTests test_app.cpp)
target_link_libraries(MyAppTests PRIVATE GTest::GTest GTest::Main SubLib)
target_include_directories(MyAppTests PRIVATE ${PROJECT_SOURCE_DIR}/src)

enable_mapfile(MyAppTests)

add_test(NAME MyAppTests COMMAND MyAppTests)