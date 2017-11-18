
include_directories(${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_SOURCE_DIR})

add_library(RookieLib bitboard.cpp bitboard.h board.cpp board.h movegen.cpp movegen.h evaluate.cpp validate.cpp evaluate.h search.cpp perft.cpp polybook.cpp polykeys.cpp polykeys.h uci.cpp xboard.cpp interface.h init.cpp ttable.cpp data.cpp io.cpp misc.cpp)

add_executable(Rookie++ Rookie++.cpp)
target_link_libraries(Rookie++ RookieLib)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
