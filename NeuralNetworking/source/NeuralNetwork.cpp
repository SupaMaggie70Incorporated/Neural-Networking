#include "pch.h"
#include "NeuralNetwork.h"
#include "DLDomain.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace SupaDL;
using namespace std;


void OptimizedNetwork::CopyTo(OptimizedNetwork* network) {
	memcpy(network->NeuronBuffer, NeuronBuffer, Domain->InputCount * Domain->OutputCount * sizeof(DataType));
}
void OptimizedNetwork::Optimize(NeuralNetwork* network) {
	
	for(int i = 0;i < Domain->InputCount;i++) {
		Domain->OptimizationInputBuffer[i] = 0;
	}
	for(int i = 0;i < Domain->InputCount;i++) {
		for(int j = 0;j < Domain->InputCount;j++) {
			Domain->OptimizationInputBuffer[j] = 0;
		}
		Domain->OptimizationInputBuffer[i] = 1;
		network->Execute(Domain->OptimizationInputBuffer, Domain->OptimizationOutputBuffer, Domain->ExecutionBuffer);
		for(int j = 0;j < Domain->OutputCount;j++) {
			NeuronBuffer[(i * Domain->OutputCount) + j] = Domain->OptimizationOutputBuffer[j];
		}
	}
}
void OptimizedNetwork::Execute(DataType* inputs, DataType* outputs) {
	for(int output = 0;output < Domain->OutputCount;output++) {
		outputs[output] = 0;
		for(int input = 0;input < Domain->InputCount;input++) {
			outputs[output] += inputs[input] * NeuronBuffer[(input * Domain->OutputCount) + output];
		}
	}
}
void OptimizedNetwork::Initialize(DLDomain* Domain) {
	this->Domain = Domain;
	this->NeuronBuffer = (DataType*)malloc(Domain->InputCount * Domain->OutputCount * sizeof(DataType));
}
OptimizedNetwork::~OptimizedNetwork() {
	if(NeuronBuffer != NULL) free(NeuronBuffer);
	NeuronBuffer = NULL;
}

NeuralNetwork::~NeuralNetwork() {
	if(NeuronBuffer != NULL) free(NeuronBuffer);
	NeuronBuffer = NULL;
}

void NeuralNetwork::Initialize(DLDomain* Domain) {
	this->Domain = Domain;
	this->NeuronBuffer = malloc(Domain->NetworkBufferSize);
}
inline int NeuralNetwork::GetBufferIndex(int layer, int neuron, int input) {
	if (layer == 0) {
		return (neuron * Domain->InputCount) + input;
	}
	else if (layer == Domain->LayersPerNetwork) {
		int startIndex = (Domain->NeuronsPerLayer * Domain->InputCount) + (Domain->NeuronsPerLayer * Domain->NeuronsPerLayer * Domain->LayersPerNetwork);
		return startIndex + (neuron * Domain->NeuronsPerLayer) + input;
	}
	else {
		int startIndex = Domain->NeuronsPerLayer * Domain->InputCount;
		return startIndex + (layer * Domain->NeuronsPerLayer * Domain->NeuronsPerLayer) + (neuron * Domain->NeuronsPerLayer) + input;
	}
}
void NeuralNetwork::Execute(DataType* inputs, DataType* outputs, DataType* execution_buffer) {
	bool IsInOutput = false;
	if (execution_buffer == NULL) execution_buffer = (DataType*)Domain->ExecutionBuffer;


	DataType* buf = (DataType*)NeuronBuffer;

	DataType* execbuf1 = execution_buffer;
	DataType* execbuf2 = execution_buffer + (sizeof(DataType) * Domain->NeuronsPerLayer);


	// Do the first layer which takes the original inputs as input and outputs the normal number of neurons
	for (int neuron = 0; neuron < Domain->NeuronsPerLayer; neuron++) {
		execbuf1[neuron] = 0;
		for (int input = 0; input < Domain->InputCount; input++) {
			execbuf1[neuron] += inputs[input] * buf[GetBufferIndex(0, neuron, input)];
			//printf("%f * %f = %f\n", inputs[input], buf[GetBufferIndex(0, neuron, input)], inputs[input] * buf[GetBufferIndex(0, neuron, input)]);
		}
		//printf("Setting layer 0 neuron %d to %f\n", neuron, execbuf1[neuron]);
	}
	// Do the in between layers which take an equal number of inputs and outputs
	for (int layer = 1; layer < Domain->LayersPerNetwork; layer++) {
		for (int neuron = 0; neuron < Domain->NeuronsPerLayer; neuron++) {
			if (layer & 1) {
				execbuf2[neuron] = 0;
				for (int input = 0; input < Domain->NeuronsPerLayer; input++) {
					execbuf2[neuron] += execbuf1[input] * buf[GetBufferIndex(layer, neuron, input)];
				}
				//printf("Setting layer %d neuron %d to %f\n", layer, neuron, execbuf2[neuron]);
			}
			else {
				execbuf1[neuron] = 0;
				for (int input = 0; input < Domain->NeuronsPerLayer; input++) {
					execbuf1[neuron] += execbuf2[input] * buf[GetBufferIndex(layer, neuron, input)];
				}
				//printf("Setting layer %d neuron %d to %f\n", layer, neuron, execbuf1[neuron]);
			}
		}
	}
	// Do the final(output) layer which takes the normal amount of inputs but a different amount of outputs
	for (int neuron = 0; neuron < Domain->OutputCount; neuron++) {
		outputs[neuron] = 0;
		if (Domain->LayersPerNetwork & 1) {
			for (int input = 0; input < Domain->NeuronsPerLayer; input++) {
				outputs[neuron] += execbuf1[input] * buf[GetBufferIndex(Domain->LayersPerNetwork, neuron, input)];
			}
		}
		else {
			for (int input = 0; input < Domain->NeuronsPerLayer; input++) {
				outputs[neuron] += execbuf2[input] * buf[GetBufferIndex(Domain->LayersPerNetwork, neuron, input)];
			}
		}
		//printf("Setting output neuron %d to %f\n", neuron, outputs[neuron]);
	}
}
void NeuralNetwork::DuplicateTo(NeuralNetwork* destination) {
	for (int i = 0; i < Domain->NetworkBufferSize; i++) {
		char value = ((char*)NeuronBuffer)[i];
		((char*)destination->NeuronBuffer)[i] = value;
	}
	//memcpy(destination->NeuronBuffer, NeuronBuffer, Domain->NetworkBufferSize);
}
void NeuralNetwork::MutateTo(NeuralNetwork* destination) {
	memcpy(destination->NeuronBuffer, NeuronBuffer, Domain->NetworkBufferSize);

	DataType* buf = (DataType*)destination->NeuronBuffer;
	int nCount = ((Domain->NeuronsPerLayer * Domain->InputCount) + (Domain->NeuronsPerLayer * Domain->NeuronsPerLayer * (Domain->LayersPerNetwork)) + (Domain->OutputCount * Domain->NeuronsPerLayer));

	for (int i = 0; i < Domain->MutationCount; i++) {
		int index = rand() % nCount;
		DataType mutationAmount = ((((double)rand()) / (RAND_MAX) * 2) - 1) * Domain->MutationAmount;
		buf[index] += mutationAmount;
	}
}
void NeuralNetwork::Mutate() {
	DataType* buf = (DataType*)NeuronBuffer;
	int nCount = ((Domain->NeuronsPerLayer * Domain->InputCount) + (Domain->NeuronsPerLayer * Domain->NeuronsPerLayer * (Domain->LayersPerNetwork)) + (Domain->OutputCount * Domain->NeuronsPerLayer));
	for (int i = 0; i < Domain->MutationCount; i++) {
		int index = rand() % nCount;
		DataType mutationAmount = ((((double)rand()) / (RAND_MAX) * 2) - 1) * Domain->MutationAmount;
		buf[index] += mutationAmount;
	}
}