if(DEFINED CTEST_ACTIVE)
    if(${CTEST_ACTIVE} STREQUAL "ON")
        message(WARNING "TESTING...")

        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.14.0  # możesz zmienić tag na nowszy gdy będzie dostępny
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)



        set(TEST_SOURCES ${SOURCES})
        list(FILTER TEST_SOURCES EXCLUDE REGEX "/main\\.cpp$")

        # adding all pure test files
        aux_source_directory(_test TEST_SOURCES)

        set(TEST_EXE_NAME "test.texe")
        add_executable(${TEST_EXE_NAME} ${TEST_SOURCES})
        target_link_libraries(${TEST_EXE_NAME} PRIVATE gtest_main ${ALL_LIBRARIES})

        # Rejestracja testów
        enable_testing()
        include(GoogleTest)
        gtest_discover_tests(${TEST_EXE_NAME})
    endif()
endif()
