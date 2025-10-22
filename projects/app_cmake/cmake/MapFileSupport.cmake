# Function to enable map file generation for a target.
# Args:
#   target - The name of the target to enable map file generation for
function(enable_mapfile target)
    set(mapfile "${CMAKE_BINARY_DIR}/${target}.map")
    if(APPLE)
        target_link_options(${target} PRIVATE "-Wl,-map,${mapfile}")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(${target} PRIVATE "-Wl,-Map=${mapfile}")
    else()
        message(WARNING "Not a GNU or LLVM linker. No idea how to do this")
    endif()
endfunction()