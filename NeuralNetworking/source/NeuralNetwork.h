#ifndef NeuralNetworkHeader
#define NeuralNetworkHeader

#include "DLDomain.h"



namespace SupaDL {
	class DLDomain;

	struct NeuralNetwork
	{
	public:
		DLDomain* Domain;

		
		void* NeuronBuffer;

		void Mutate();
		void MutateTo(NeuralNetwork* destination);
		void DuplicateTo(NeuralNetwork* destination);

		void Execute(DataType* inputs, DataType* outputs, DataType* execution_buffer);

		inline int GetBufferIndex(int layer, int neuron, int input);

		void Initialize(DLDomain* Domain);
		~NeuralNetwork();
	};
	struct OptimizedNetwork {
	public:
		DLDomain* Domain;
		void* NeuronBuffer;
		void Optimize(NeuralNetwork* network);
		void CopyTo(OptimizedNetwork* network);
		void Initialize(DLDomain domain);
		~OptimizedNetwork();
	};
}

#endif