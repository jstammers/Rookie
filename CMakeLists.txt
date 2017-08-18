cmake_minimum_required(VERSION 3.0.0)
project(Rookie VERSION 1.0)

include_directories(${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_SOURCE_DIR})

set(PROGSRC bitboard.c init.c hashkeys.c data.c board.c attack.c io.c validate.c movegen.c makemove.c perft.c search.c misc.c pvtable.c evaluate.c uci.c xboard.c polybook.c polykeys.c)

include(CTest)
enable_testing()

add_executable(Rookie Rookie.c ${PROGSRC})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
