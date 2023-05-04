#ifndef OthelloDLManagerHeader
#define OthelloDLManagerHeader

#include <DeepLearning.h>
#include "OthelloBoard.h"



using namespace SupaDL;

struct OthelloDLManager {
public:
	DataType InputBuffer[64];
	DataType OutputBuffer[3];

	OthelloBoard* Board;
	DLDomain* Domain;
	DataType* NeuralNetworkExecutionBuffer;
	NeuralNetwork* Network1;
	NeuralNetwork* Network2;

	OthelloDLManager() {
	}
	/// <summary>
	/// 0 means draw, 1 means player 1 won, 2 means player 2 won
	/// </summary>
	/// <returns></returns>
	signed char Play();
	signed char PlayPair();
	char PlayMove();
	void Initialize(DLDomain* domain, NeuralNetwork* network1, NeuralNetwork* network2, DataType* ExecutionBuffer, OthelloBoard* board);
};

#endif