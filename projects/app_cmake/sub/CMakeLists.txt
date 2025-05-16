add_library(SubLib SHARED lib.cpp)
target_include_directories(SubLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})