# adding all internal libraries
add_subdirectory(_libs/math)
set(ALL_LIBRARIES ${ALL_LIBRARIES} internal_lib_math)



MESSAGE(STATUS "Downloading external libraries...")

# 1 - json #
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        master
)
# FetchContent_Populate(json)      # pobiera
FetchContent_MakeAvailable(json)   # pobiera i buduje ==> Populate + add_subdirectory

set(ALL_LIBRARIES ${ALL_LIBRARIES} nlohmann_json::nlohmann_json)
include_directories(${json_SOURCE_DIR}/include/nlohmann)

MESSAGE(STATUS "JSON")


# 2 - core #

include(FetchContent)
FetchContent_Declare(
    core
    GIT_REPOSITORY https://github.com/McEnigma24/CORE_lib.git
    GIT_TAG        main # lub inny tag/commit
)
FetchContent_Populate(core)

# set(ALL_LIBRARIES ${ALL_LIBRARIES} core)
include_directories(${core_SOURCE_DIR}/_inc)

MESSAGE(STATUS "CORE")
MESSAGE(STATUS "core_SOURCE_DIR=${core_SOURCE_DIR}")



MESSAGE(STATUS "DONE: ALL_LIBRARIES=${ALL_LIBRARIES}")