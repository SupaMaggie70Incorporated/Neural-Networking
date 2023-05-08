#include "OthelloDLManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <new>



/// <summary>
/// Positive is in favor of player 1, negative in favor of player 2
/// </summary>
/// <returns></returns>
signed char OthelloDLManager::PlayPair(int evaluationsPerMove) {
}

/// <summary>
/// Change this to play a pair of games, and play some random number of moves from the start so it isnt the same game every time
/// </summary>
/// <returns></returns>
signed char OthelloDLManager::Play(int evaulationsPerMove) {
}
signed char OthelloDLManager::PlayTrainingPair(int evaluationsPerMove) {

}
inline DataType GetTypeFromChar(char piece, char turn) {
	if (piece == turn) return 1;
	else if (piece == NOPIECE) return 0;
	else return -1;
}
char OthelloDLManager::PlayMove(int evaluations) {
}

void OthelloDLManager::Initialize(DLDomain* domain, NeuralNetwork* network1, NeuralNetwork* network2, DataType* executionBuffer) {
	this->Domain = domain;
	this->Network1 = network1;
	this->Network2 = network2;
	this->NeuralNetworkExecutionBuffer = executionBuffer;
	Position.Initialize();
}
void OthelloDLManager::InitSearchTree(int hashSize) {
	Tree = new SearchTree(hashSize, &Position);
}