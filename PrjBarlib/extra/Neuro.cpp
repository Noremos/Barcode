//#include "CellAutomat.h"
//
//void Neuron::AddConnection(uint NUM, float WEIGHT)
//{
//	allConnections[numConnections].num = NUM;
//	allConnections[numConnections++].weight = WEIGHT;
//}
//
//bool Neuron::IsConnected(uint index)
//{
//	for (uint i = 0; i < numConnections; ++i)
//	{
//		if (allConnections[i].num == index)
//			return true;
//	}
//
//	return false;
//}
//
//void Neuron::ClearConnections()
//{
//	numConnections = 0;
//}
//
//void Neuron::SetRandomBias()
//{
//	bias = (RandomVal(40001) * 0.0001f) - 2.0f;
//}
//
//void Neuron::SetRandomType()
//{
//	if ((type != input) && (type != output))
//	{
//		if (RandomPercent(2))
//		{
//			type = random;
//
//			return;
//		}
//
//#ifdef UseMemoryNeuron
//		int refVal = RandomVal(15);
//#else
//		int refVal = RandomVal(14);
//#endif
//
//		if (refVal <= 8)
//			type = basic;
//		else if (refVal <= 13)
//			type = radialbasis;
//		else
//			type = memory;
//
//	}
//}
//
//void Neuron::SetRandomConnections()
//{
//
//	ClearConnections();
//
//	if (type == output)
//		return;
//
//	uint connections = RandomVal(MaxConnectionsPerNeuron + 1);
//
//	if (connections > 0)
//	{
//		uint connectionIndex;
//
//		for (;;)
//		{
//			connectionIndex = RandomVal(NeuronsInLayer);
//
//			if (!IsConnected(connectionIndex))
//			{
//				AddConnection(connectionIndex, (RandomVal(40001) * 0.0001f) - 2.0f);
//
//				if (--connections == 0)
//					return;
//			}
//		}
//	}
//}
//
//void Neuron::SetRandom()
//{
//	SetRandomBias();
//	SetRandomType();
//	SetRandomConnections();
//}
//
//void Neuron::SetZero()
//{
//	ClearConnections();
//	bias = 0.0f;
//}
//
//void Neuron::SetTunnel(int num)
//{
//	bias = 0.0f;
//	numConnections = 1;
//	allConnections[0].weight = 1.0f;
//	allConnections[0].num = num;
//}
//
//void Neuron::SlightlyChange()
//{
//	bias += (RandomVal(1001) * 0.0001f) - .05f;
//
//	for (uint i = 0; i < numConnections; ++i)
//	{
//		allConnections[i].weight += (RandomVal(2001) * 0.0001f) - .1f;
//	}
//}
//
//void Neuron::SortConnections()
//{
//	//TODO
//
//}
//
//float BotNeuralNet::Activation(float value)
//{
//	return (value >= 0.5f) ? 1.0f : 0.0f;
//}
//
//float BotNeuralNet::PlusMinusActivation(float value)
//{
//	if (value >= 0.5f)
//		return 1.0f;
//	else if (value <= -0.5f)
//		return -1.0f;
//
//	return 0.0f;
//}
//
//float BotNeuralNet::RadialBasisActivation(float value)
//{
//	return ((value >= 0.45f) && (value <= 0.55f)) ? 1.0f : 0.0f;
//}
//
//void BotNeuralNet::ClearMemory()
//{
//	memset(allMemory, 0, sizeof(allMemory));
//}
//
//
//BotNeuralNet::BotNeuralNet()
//{
//	for (uint yi = 0; yi < NeuronsInLayer; ++yi)
//	{
//		allNeurons[NeuronOutputLayerIndex][yi].type = output;
//		allNeurons[NeuronInputLayerIndex][yi].type = input;
//	}
//
//	ClearMemory();
//}
//
//BotNeuralNet::BotNeuralNet(BotNeuralNet* prototype)
//{
//	BotNeuralNet();
//
//	ClearMemory();
//	memcpy(allNeurons, prototype->allNeurons, sizeof(allNeurons));
//
//}
//
//void BotNeuralNet::Clear()
//{
//	memset(allValues, 0, sizeof(allValues));
//}
//
//void BotNeuralNet::Process()
//{
//	float* value;
//	float* m;
//	Neuron* n;
//	int t, v;
//
//	for (uint xi = 0; xi < NumNeuronLayers - 1; ++xi)
//	{
//		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
//		{
//			value = &allValues[xi][yi];
//			m = &allMemory[xi][yi];
//			n = &allNeurons[xi][yi];
//
//			//Skip calculation if neuron has no further connections
//			if (n->numConnections == 0)
//			{
//				continue;
//			}
//
//			switch (n->type)
//			{
//			case basic:
//				*value = Activation(*value + n->bias);
//				break;
//
//			case random:
//				t = RandomVal(1000);
//				v = (int)((*value + n->bias) * 1000.0f);
//				*value = (t <= v) ? 1.0f : 0.0f;
//				break;
//
//			case input:
//				*value = *value + n->bias;
//				break;
//
//			case radialbasis:
//				*value = RadialBasisActivation(*value + n->bias);
//				break;
//
//			case memory:
//				if ((*value + n->bias) <= -.5f)
//				{
//					//Wipe memory
//					*m = 0.0f;
//				}
//				else if ((*value + n->bias) >= .5f)
//				{
//					//Write in memory
//					*m += Activation(*value + n->bias);
//
//					//Maximal value
//					if (*m > 5.0f)
//						*m = 5.0f;
//				}
//				*value = *m;
//				break;
//			}
//
//			for (uint i = 0; i < n->numConnections; ++i)
//			{
//				allValues[xi + 1][n->allConnections[i].num] += *value * n->allConnections[i].weight;
//			}
//		}
//	}
//
//
//	//Output layer
//
//	//Rotation
//	value = &allValues[NeuronOutputLayerIndex][0];
//	n = &allNeurons[NeuronOutputLayerIndex][0];
//
//	*value = PlusMinusActivation(*value + n->bias);
//
//	//All the rest
//	for (uint oi = 1; oi < NeuronsInLayer; ++oi)
//	{
//		value = &allValues[NeuronOutputLayerIndex][oi];
//		n = &allNeurons[NeuronOutputLayerIndex][oi];
//
//		*value = Activation(*value + n->bias);
//	}
//}
//
//
//void BotNeuralNet::MutateSlightly()
//{
//	uint randomNeuronIndex = RandomVal(NumNeuronLayers * NeuronsInLayer);
//	uint counter = 0;
//
//	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
//	{
//		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
//		{
//
//			if (counter++ == randomNeuronIndex)
//			{
//				Neuron* n = &allNeurons[xi][yi];
//
//				n->SlightlyChange();
//			}
//
//		}
//	}
//}
//
//
//void BotNeuralNet::Mutate()
//{
//	uint randomNeuronIndex = RandomVal(NumNeuronLayers * NeuronsInLayer);
//	uint counter = 0;
//
//	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
//	{
//		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
//		{
//			if (counter++ == randomNeuronIndex)
//			{
//				Neuron* n = &allNeurons[xi][yi];
//
//				n->SetRandom();
//			}
//		}
//	}
//}
//
//
//void BotNeuralNet::MutateHarsh()
//{
//	for (uint xi = 0; xi < NumNeuronLayers; ++xi)
//	{
//		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
//		{
//
//			if (RandomPercent(50))
//				continue;
//
//			Neuron* n = &allNeurons[xi][yi];
//
//			n->SetRandom();
//
//		}
//	}
//}
//
//
//
//void BotNeuralNet::Randomize()
//{
//	for (uint y = 0; y < NeuronsInLayer; ++y)
//	{
//		allNeurons[NeuronInputLayerIndex][y].SetRandomConnections();
//		allNeurons[NeuronInputLayerIndex][y].SetRandomBias();
//	}
//
//	for (uint xi = 1; xi < NumNeuronLayers - 1; ++xi)
//	{
//		for (uint yi = 0; yi < NeuronsInLayer; ++yi)
//		{
//			allNeurons[xi][yi].SetRandom();
//		}
//	}
//
//	for (uint y = 0; y < NeuronsInLayer; ++y)
//	{
//		allNeurons[NeuronOutputLayerIndex][y].SetRandomBias();
//	}
//}
//
//
//BrainOutput BotNeuralNet::GetOutput()
//{
//	BrainOutput toRet;
//
//	repeat(BrainOutput::numFields)
//		toRet.fields[i] = (int)allValues[NeuronOutputLayerIndex][i];
//
//	return toRet;
//}
//
//
//void BotNeuralNet::SetDummy()
//{
//	allNeurons[NeuronOutputLayerIndex][2].bias = 0.5f;
//	allNeurons[NeuronInputLayerIndex][0].SetTunnel(3);
//	allNeurons[1][3].SetTunnel(3);
//	allNeurons[2][3].SetTunnel(3);
//	allNeurons[3][3].SetTunnel(3);
//	allNeurons[4][3].bias = 0.0f;
//	allNeurons[4][3].AddConnection(3, 0.5f);
//}