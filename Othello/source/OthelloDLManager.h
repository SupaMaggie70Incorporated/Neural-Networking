#ifndef OthelloDLManagerHeader
#define OthelloDLManagerHeader

#include <DeepLearning.h>
#include "OthelloBoard.h"
#include "SearchTree.h"



using namespace SupaDL;

struct OthelloDLManager {
public:
	DataType InputBuffer[64];
	DataType OutputBuffer[3];
	BoardPosition Position;
	DLDomain* Domain;
	DataType* NeuralNetworkExecutionBuffer;
	OptimizedNetwork* Network1;
	OptimizedNetwork* Network2;
	SearchTree* Tree;

	/// <summary>
	/// 0 means draw, 1 means player 1 won, 2 means player 2 won
	/// </summary>
	/// <returns></returns>
	StaticEvalSet Play(int evaluationsPerMove);
	signed char PlayPair(int evaluationsPerMove);
	signed char PlayTrainingPair(int evaluationsPerMove);
	void PlayMove(int evaluations);
	void InitSearchTree(int hashsize);
	void Initialize(DLDomain* domain, OptimizedNetwork* network1, OptimizedNetwork* network2, DataType* executionBuffer);
};

#endif