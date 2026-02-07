cmake_minimum_required(VERSION 3.20)
project(app_7 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CONST_TARGET_NAME "itf_generator")

include_directories(_core/_inc)
aux_source_directory(_core/_src SOURCES)

include(_cmake/C-requirements.cmake)
include(_cmake/C-commands.cmake)
include(_cmake/C-flags.cmake)
include(_cmake/C-libs.cmake)
include(_cmake/C-test.cmake)
include(_cmake/C-macros.cmake)

add_executable(${CONST_TARGET_NAME} ${SOURCES})
target_link_libraries(${CONST_TARGET_NAME} PRIVATE ${ALL_LIBRARIES})
target_compile_definitions(${CONST_TARGET_NAME} PRIVATE ${MACRO_LIST})

# Custom target to run the executable
add_custom_target(run_default
    COMMAND $<TARGET_FILE:${CONST_TARGET_NAME}>
    DEPENDS ${CONST_TARGET_NAME}
    COMMENT "Running ${CONST_TARGET_NAME}..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

include(_cmake/C-packing.cmake)


# get_filename_component(FIRMWARE_DIR "${CMAKE_CURRENT_LIST_DIR}/../firmware" ABSOLUTE)