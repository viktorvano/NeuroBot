# NeuroBot
This is open source robot with deep neural network.
Deep Neural Network can be configured by a file content.

Training Data example:
	Inputs						                Outputs	
	{ 1.0f, 1.0f, 0.0f, 1.0f, 1.0f },	{ 0.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },	{ 1.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f, 1.0f, 1.0f },	{ 1.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },	{ 0.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },	{ 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },	{ 1.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },	{ 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },	{ 0.0f, 0.0f },
	{ 0.5f, 0.5f, 0.5f, 0.5f, 0.5f },	{ 0.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },	{ 1.0f, 1.0f },

Topology example:
input layer: 5, 	
hidden layer: 9,
hidden layer: 8,
output layer: 2, 

If the content of weights.txt file is deleted, neural network automatically retrain itself again.
If the number of weights in the weights.txt file matches with topology, the Neural Network just loads them, and runs.
