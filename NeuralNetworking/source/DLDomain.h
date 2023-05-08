#ifndef DLDomainHeader
#define DLDomainHeader


namespace SupaDL {
	class NeuralNetwork;
	
	typedef double DataType;
	class DLDomain
	{
	public:
		/// <summary>
		/// The number of layers in every network, not counting the final output layer
		/// </summary>
		int LayersPerNetwork;
		int NeuronsPerLayer;
		int NeuronTotal;


		int InputCount;
		int OutputCount;



		// Unused, and not implemented as of yet
		int NetworkCount;
		int MaxNetworks;
		NeuralNetwork* Networks;

		DataType* ExecutionBuffer;
		int ExecutionBufferSize;
		int NetworkBufferSize;

		DataType MutationAmount;
		int MutationCount;


		inline DataType* MakeExecutionBuffer();
		DLDomain(int layers, int neuronsperlayer, int max_networks, int inputcount, int outputcount, DataType mutationAmount, int mutationCount);
	};
}

#endif