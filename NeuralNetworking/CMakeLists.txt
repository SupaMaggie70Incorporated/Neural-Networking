project(NeuralNetworking VERSION 1.0 DESCRIPTION "This is the description")

set(CMAKE_BUILD_TYPE "Debug")
include(GNUInstallDirs)

add_library(NeuralNetworking STATIC
    source/DeepLearning.cpp
    source/DLDomain.cpp
    source/NeuralNetwork.cpp
    source/pch.cpp

    source/DeepLearning.h
    source/DLDomain.h
    source/framework.h
    source/NeuralNetwork.h
    source/pch.h
)

target_include_directories(NeuralNetworking PUBLIC source)
set_target_properties(NeuralNetworking PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(NeuralNetworking PROPERTIES PUBLIC_HEADER source/DeepLearning.h)