# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra")                                                             # to see everything
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wfatal-errors -Werror=uninitialized -Werror=init-self -Werror=reorder -Wdelete-incomplete")    # for normal coding
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -w")                                                         # for release

# Check environment variable and CMake cache variable
if(DEFINED ENV{FATAL_ERRORS_FLAG})
    if($ENV{FATAL_ERRORS_FLAG} STREQUAL "ON")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")
    endif()
endif()

if(DEFINED FATAL_ERRORS_FLAG)
    if(${FATAL_ERRORS_FLAG} STREQUAL "ON")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")   # ASSEMBLY + PREPROCESSED and other
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -S") # ASSEMBLY
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -S -fverbose-asm") # ASSEMBLY with comments

# cmake_examples/3_internal_lib_A/build/CMakeFiles/app_3.dir/_core/_src/main.cpp.ii
