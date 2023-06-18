project(Othello DESCRIPTION "This is the description")

include(GNUInstallDirs)
include(CTest)
include_directories(../NeuralNetworking/source)
enable_testing()

add_executable(Othello
    source/Othello.cpp
    source/OthelloBoard.cpp
    source/OthelloDLManager.cpp
    source/SearchTree.cpp

    source/Othello.h
    source/OthelloBoard.h
    source/OthelloDLManager.h
    source/SearchTree.h

    #source/FakeOthello.cpp
)

target_link_libraries(Othello PUBLIC NeuralNetworking)