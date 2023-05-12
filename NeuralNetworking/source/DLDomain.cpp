#include "pch.h"
#include "DLDomain.h"
#include "NeuralNetwork.h"
#include <stdlib.h>
#include <stdio.h>

using namespace SupaDL;

DLDomain::DLDomain(int layers, int neuronsperlayer, int max_networks, int inputcount, int outputcount, DataType mutationAmount, int mutationCount) {
	if (layers == 0 || neuronsperlayer == 0 || max_networks == 0 || inputcount == 0 || outputcount == 0) {
		printf("Zero value passed to DLDomain constructor");
		exit(EXIT_FAILURE);
	}
	this->LayersPerNetwork = layers;
	this->NeuronsPerLayer = neuronsperlayer;

	this->InputCount = inputcount;
	this->OutputCount = outputcount;

	this->NetworkBufferSize = ((neuronsperlayer * inputcount) + (neuronsperlayer * neuronsperlayer * (layers)) + (outputcount * neuronsperlayer)) * sizeof(DataType);

	this->MaxNetworks = max_networks;
	this->Networks = (NeuralNetwork*)malloc(max_networks * sizeof(NeuralNetwork));
	for (int i = 0; i < MaxNetworks; i++) {
		Networks[i].Initialize(this);
	}
	int nCount = ((NeuronsPerLayer * InputCount) + (NeuronsPerLayer * NeuronsPerLayer * (LayersPerNetwork)) + (OutputCount * NeuronsPerLayer));
	for (int i = 0; i < nCount; i++) {
		((DataType*)(Networks[0].NeuronBuffer))[i] = 0;
	}
	for (int i = 1; i < MaxNetworks; i++) {
		Networks[0].DuplicateTo(&Networks[i]);
	}
	this->NetworkCount = 1;


	NeuronTotal = (NeuronsPerLayer * LayersPerNetwork) + OutputCount;
	this->ExecutionBufferSize = NeuronsPerLayer * 2 * sizeof(DataType); // We don't need to include the input or output neurons in the execution buffer
	this->ExecutionBuffer = MakeExecutionBuffer();
	this->MutationAmount = mutationAmount;
	this->MutationCount = mutationCount;

	OptimizationInputBuffer = (DataType*)malloc(InputCount * sizeof(DataType));
	OptimizationOutputBuffer = (DataType*)malloc(OutputCount * sizeof(DataType));
}
inline DataType* DLDomain::MakeExecutionBuffer() {
	return (DataType*)malloc(ExecutionBufferSize);
}