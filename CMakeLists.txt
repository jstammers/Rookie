cmake_minimum_required(VERSION 3.0.0)
project(Rookie VERSION 1.0)

include_directories(${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_SOURCE_DIR})

set(PROGSRC bitboard.cpp init.cpp hashkeys.cpp data.cpp board.cpp attack.cpp io.cpp validate.cpp movegen.cpp makemove.cpp perft.cpp search.cpp misc.cpp pvtable.cpp evaluate.cpp uci.cpp xboard.cpp polybook.cpp polykeys.cpp)

include(CTest)
enable_testing()

add_executable(Rookie++ Rookie++.cpp ${PROGSRC})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
