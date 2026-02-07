# Check if git is initialized in the repository
set(GIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.git")
if(IS_DIRECTORY "${GIT_DIR}" OR EXISTS "${GIT_DIR}")
    # Git repository is initialized, check if git command is available
    find_program(GIT_EXECUTABLE git)
    if(GIT_EXECUTABLE)
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #



        # Git is available and repo is initialized - update submodules
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT
            OUTPUT_QUIET
            ERROR_QUIET
            )
            if(GIT_SUBMOD_RESULT EQUAL 0)
            message(STATUS "Git submodule updated successfully")
            else()
            message(WARNING "Failed to update git submodule (result: ${GIT_SUBMOD_RESULT})")
            endif()



    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    else()
        message(STATUS "Git executable not found")
    endif()
else()
    message(STATUS "Git repository not initialized")
endif()
