cmake_minimum_required(VERSION 3.20)
project(InternalLib LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CONST_TARGET_NAME "internal_lib_math")

# # adding all includes
# include_directories(_core/_inc)          # not needed because target_include_directories

# # adding all sources
aux_source_directory(_core/_src SOURCES)   # this works but it requires dummy main.cpp file

# file(GLOB SOURCES CONFIGURE_DEPENDS
#     "_core/_src/*.cpp"
# )

# add_library(${CONST_TARGET_NAME} STATIC ${SOURCES})
# add_library(${CONST_TARGET_NAME} SHARED ${SOURCES})
add_library(${CONST_TARGET_NAME} OBJECT ${SOURCES})    # nie tworzy żadnych plików wynikowych, tylko internalowo dostępne pliki .o

target_include_directories(${CONST_TARGET_NAME}

    PUBLIC      # -> konsument dostaje nagłówki i skompilowaną bibliotekę

    # INTERFACE # -> jedyne co jest widoczne dla konsumentów to nagłówki
    #                    dlatego np. będziemy mieli undefined reference do funkcji, które mają definicje w .cpp
    #                       bo jedyne co dostaniemy ich deklaracje w nagłówkach

    # PRIVATE   # -> nic nie propagujey do konsumentów

    ${CMAKE_CURRENT_SOURCE_DIR}/_core/_inc
)
