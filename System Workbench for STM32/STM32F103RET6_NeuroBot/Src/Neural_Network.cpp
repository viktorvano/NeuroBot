// Neural_Network.cpp : Defines the entry point for the console application.
//

// Created by: Viktor Vano

extern "C"
{
	#include "NeuroBot.h"
}
#include <cmath>
#include <vector>
#include <iostream>
#include <cassert>
#include <sstream>
#include <stdio.h>//C library
#include "ff.h"
#include <string.h>


/*************************************************************************************************/
// Free to edit
uint16_t PatternCount;
uint16_t InputNodes;
uint16_t OutputNodes;
#define velocity 0.1; // overall net learning rate [0.0..1.0]
#define momentum 0.5; // momentum multiplier of last deltaWeight [0.0..n]
#define RunModeQuantization 5 // Number 2 generates 0, 1; Number 5 generates 0.0, 0.25, 0.5, 0.75, 1.0
/*#ifndef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif*/

using namespace std;

vector<unsigned> Topology;
vector< vector<float> > LearningInputs;
vector< vector<float> > LearningOutputs;
vector<float> Weights;
uint16_t NeuronIndex = 0;


void start_NN();
extern "C" void START_THE_NN();

void START_THE_NN()
{
	start_NN();
}

void push_zeros_to_Weights()
{
	vector <float> InputRow;
	vector <float> OutputRow;
	uint16_t index, NumberOfWeights = 0;
	uint16_t topologySize = Topology.size();

	for (index = 0; index < topologySize - 1; index++)
	{
		NumberOfWeights += (Topology[index] + 1)*Topology[index + 1];
	}

	for (index = 0; index < NumberOfWeights; index++)
	{
		Weights.push_back(0.0f);
	}
}

void push_zeros_to_Learning_table()
{
	vector <float> InputRow;
	vector <float> OutputRow;
	uint16_t row, column;

	for (row = 0; row < InputNodes; row++)
	{
		InputRow.push_back(0.0f);
	}
	for (column = 0; column < PatternCount; column++)
	{
		LearningInputs.push_back(InputRow);
	}

	for (row = 0; row < OutputNodes; row++)
	{
		OutputRow.push_back(0.0f);
	}
	for (column = 0; column < PatternCount; column++)
	{
		LearningOutputs.push_back(OutputRow);
	}
}

void push_number_to_topology(unsigned number)
{
	Topology.push_back(number);
}

extern "C" void get_training_data_count()
{
	FIL myFile;
	FRESULT status_open=FR_LOCKED, status_rw=FR_DENIED, status_close=FR_INVALID_OBJECT;
	UINT byteCount=0;
	status_open=f_open(&myFile, "training.txt", FA_READ);
	char c;

	if (status_open!=FR_OK)
	{
		printf("No File!");
		exit(7);
	}

	while (1)
	{
		status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
		if(status_rw!=FR_OK)
		{
			display_message("training.txt", "status_rw", "ERROR!");
			NeuroBot_sleep();
		}
		if (c == '}')
			PatternCount++;

		if (c == EOF || byteCount==0)
		{
			if (PatternCount % 2 == 0)
				PatternCount /= 2;
			else
			{
				printf("\nERROR: Training input data count do not match with output data count.\n");
				exit(8);
			}
			break;
		}
	}

	status_close=f_close(&myFile);//fclose(fptr);
	if(status_close!=FR_OK)
	{
		display_message("training.txt", "status_close", "ERROR!");
		NeuroBot_sleep();
	}
};

extern "C" void load_topology()
{
	FIL myFile;
	DWORD fptr_last=0;
	FRESULT status_open=FR_LOCKED, status_rw=FR_DENIED, status_close=FR_INVALID_OBJECT;
	UINT byteCount=0;
	status_open=f_open(&myFile, "topology.txt", FA_READ);
	char c;
	char str_number[5];
	unsigned index = 0, number;
	memset(str_number, 0, sizeof(str_number));

	if (status_open!=FR_OK)
	{
		//display_message("topology.txt", "status_open", "ERROR!");//printf("No File!");
		NeuroBot_sleep();
	}

	while (1)
	{
		status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
		if(status_rw!=FR_OK)
		{
			display_message("topology.txt", "status_rw", "ERROR!");
			NeuroBot_sleep();
		}
		if (c == EOF || byteCount==0)
		{
			status_close=f_close(&myFile);//fclose(fptr);
			return;
		}
		if (c >= '0' && c <= '9')
		{
			myFile.fptr = fptr_last;
			str_number[index] = c;
			if (index < sizeof(str_number))
				index++;
			break;
		}
		fptr_last = myFile.fptr;
	}

	while (1)
	{
		status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
		if(status_rw!=FR_OK)
		{
			display_message("topology.txt", "status_rw", "ERROR!");
			NeuroBot_sleep();
		}
		if (c == EOF || byteCount==0)
			break;

		if (c >= '0' && c <= '9')
		{
			str_number[index] = c;
			if (index < sizeof(str_number))
				index++;
		}

		if (c != EOF && byteCount!=0 && !(c >= '0' && c <= '9'))
		{
			memset(&str_number[strlen(str_number)-1], 0, 1);
			number = atoi(str_number);
			push_number_to_topology(number);//push value to vector topology
			index = 0;
			memset(str_number, 0, sizeof(str_number));
			while (1)
			{
				status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
				if(status_rw!=FR_OK)
				{
					display_message("topology.txt", "status_rw", "ERROR!");
					NeuroBot_sleep();
				}
				if (c == EOF || byteCount==0)
				{
					status_close=f_close(&myFile);//fclose(fptr);
					if(status_close!=FR_OK)
					{
						display_message("topology.txt", "status_close", "ERROR!");
						NeuroBot_sleep();
					}
					InputNodes = Topology[0];
					OutputNodes = Topology[Topology.size() - 1];
					get_training_data_count();
					push_zeros_to_Learning_table();
					push_zeros_to_Weights();
					return;
				}
				if (c >= '0' && c <= '9')
				{
					myFile.fptr = fptr_last;
					str_number[index] = c;
					if (index < sizeof(str_number))
						index++;
					break;
				}
				fptr_last = myFile.fptr;
			}
		}
	}
	status_close=f_close(&myFile);//fclose(fptr);
	if(status_close!=FR_OK)
	{
		display_message("topology.txt", "status_close", "ERROR!");
		NeuroBot_sleep();
	}

	InputNodes = Topology[0];
	OutputNodes = Topology[Topology.size() - 1];
	get_training_data_count();
	push_zeros_to_Learning_table();
	push_zeros_to_Weights();
};

extern "C" void load_training_data_from_file()
{
	FIL myFile;

	FRESULT status_open=FR_LOCKED, status_rw=FR_DENIED, status_close=FR_INVALID_OBJECT;
	UINT byteCount=0;
	status_open=f_open(&myFile, "training.txt", FA_READ);
	char c;
	char string[30];
	uint8_t index = 0;
	memset(string, 0, sizeof(string));
	uint16_t row = 0, column = 0;

	if (status_open!=FR_OK)
	{
		display_message("training.txt", "status_open", "ERROR!");//printf("No File!");
		exit(7);
	}

	for (column = 0; column < PatternCount; column++)
	{
		for (row = 0; row < InputNodes; row++)
		{
			while (1)
			{
				status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
				if(status_rw!=FR_OK)
				{
					display_message("training.txt", "status_rw", "ERROR!");
					NeuroBot_sleep();
				}
				if (c >= '0' && c <= '9')
				{
					string[index++] = c;
					break;
				}
			}

			while (1)
			{
				status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
				if(status_rw!=FR_OK)
				{
					display_message("training.txt", "status_rw", "ERROR!");
					NeuroBot_sleep();
				}
				if (c == ',')
				{
					LearningInputs[column][row] = atof(string);
					index = 0;
					break;
				}
			}
		}

		for (row = 0; row < OutputNodes; row++)
		{
			while (1)
			{
				status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
				if (c >= '0' && c <= '9')
				{
					string[index++] = c;
					break;
				}
			}

			while (1)
			{
				status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
				if (c == ',')
				{
					LearningOutputs[column][row] = atof(string);
					index = 0;
					break;
				}
			}
		}
	}

	status_close=f_close(&myFile);//fclose(fptr);
	if(status_close!=FR_OK)
	{
		display_message("training.txt", "status_close", "ERROR!");
		NeuroBot_sleep();
	}
};

extern "C" uint16_t get_number_of_weights_from_file()
{
	uint16_t number_of_weights = 0;

	FIL myFile;

	FRESULT status_open=FR_LOCKED, status_rw=FR_DENIED, status_close=FR_INVALID_OBJECT;
	UINT byteCount=0;
	char c;

	do
	{
		status_open=f_open(&myFile, "weights.txt", FA_READ);
		if(status_open!=FR_OK)
		{
			while(f_mount(&myFAT, SD_Path, 0)!=FR_OK);
			while(f_mount(&myFAT, SD_Path, 1)!=FR_OK);
		}
	}while(status_open!=FR_OK);

	while (1)
	{
		status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
		if(status_rw!=FR_OK)
		{
			display_message("weights.txt", "status_rw", "ERROR!");
			NeuroBot_sleep();
		}
		if (c == '\n' && byteCount==1)
		{
			number_of_weights++;
		}
		else if (c == EOF || byteCount==0)
		{
			break;
		}
	}

	status_close=f_close(&myFile);//fclose(fptr);
	if(status_close!=FR_OK)
	{
		display_message("weights.txt", "status_close", "ERROR!");
		NeuroBot_sleep();
	}

	return number_of_weights;
};

// reverses a string 'str' of length 'len'
extern "C" void reverse(char *str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
};

// Converts a given integer x to string str[].  d is the number
// of digits required in output. If d is more than the number
// of digits in x, then 0s are added at the beginning.
extern "C" int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
		str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
};

// Converts a floating point number to string.
extern "C" void ftoa(float n, char *res, int afterpoint)
{
	unsigned char minus_flag = 0;
	if (n < 0)
	{
		minus_flag = 1;
		n = -n;
	}

	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// convert integer part to string
	int i = intToStr(ipart, res, 0);

	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot

					   // Get the value of fraction part upto given no.
					   // of points after dot. The third parameter is needed
					   // to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);

		intToStr((int)fpart, res + i + 1, afterpoint);
	}

	char string[30];
	if (minus_flag == 1)
	{
		memset(string, 0, 30);
		string[0] = '-';
		if (n < 1.0f)
		{
			string[1] = '0';
			strcpy(&string[2], res);
		}
		else
			strcpy(&string[1], res);

		memset(res, 0, strlen(res));
		strcpy(res, string);
	}
	else
		if (n < 1.0f)
		{
			string[0] = '0';
			strcpy(&string[1], res);
			memset(res, 0, strlen(res));
			strcpy(res, string);
		}



};

/*const float LearningInputs[PatternCount][InputNodes] = {
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
};

const float LearningOutputs[PatternCount][OutputNodes] = {
	{ 0.0f, 1.0f },// LearningOutputs[x][0] represents AND for LearningInputs[x][0,1]
	{ 0.0f, 0.0f },// LearningOutputs[x][1] represents XNOR for LearningInputs[x][0,1,2,3]
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 1.0f, 1.0f },
};*/
/**********************************************************************************************/

using namespace std;

int training_line = 0;// Has to be initialized 0
vector<float> input, target, result;
//int trainingPass = 0;

// Training class to read training data from an array
class TrainingData
{
public:
	unsigned getNextInputs(vector<float> &inputValues);
	unsigned getTargetOutputs(vector<float> &targetOutValues);
};

unsigned TrainingData::getNextInputs(vector<float> &inputValues)
{
	inputValues.clear();

	if (training_line >= PatternCount)
		training_line = 0;

	for (int i = 0; i < InputNodes; i++)
		inputValues.push_back(LearningInputs[training_line][i]);

	return inputValues.size();
}

unsigned TrainingData::getTargetOutputs(vector<float> &targetOutValues)
{
	targetOutValues.clear();

	for (int i = 0; i < OutputNodes; i++)
		targetOutValues.push_back(LearningOutputs[training_line][i]);

	training_line++;

	return targetOutValues.size();
}

struct Connection
{
	float weight;
	float deltaWeight;
};

class Neuron;

typedef vector<Neuron> Layer;

//************ class Neuron **************

class Neuron
{
public:
	Neuron(unsigned numOutputs, unsigned myIndex);
	void setOutputValue(float value) { m_outputValue = value; }
	float getOutputValue(void) const { return m_outputValue; }
	void feedForward(const Layer &prevLayer);
	void calcOutputGradients(float targerValue);
	void calcHiddenGradients(const Layer &nextLayer);
	void updateInputWeights(Layer &prevLayer);
	void saveInputWeights(Layer &prevLayer);
	void loadInputWeights(Layer &prevLayer);

private:
	static float eta; // [0.0..1.0] overall network training rate
	static float alpha; // [0.0..n] multiplier of last weight change (momentum)
	static float transferFunction(float x);
	static float transferFunctionDerivative(float x);
	static float randomWeight(void);
	float sumDOW(const Layer &nextLayer) const;
	float m_outputValue;
	vector<Connection> m_outputWeights;
	unsigned m_myIndex;
	float m_gradient;
};

Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
	for (unsigned c = 0; c < numOutputs; c++)
	{
		m_outputWeights.push_back(Connection());
		m_outputWeights.back().weight = randomWeight();
	}

	m_myIndex = myIndex;
}

void Neuron::feedForward(const Layer &prevLayer)
{
	float sum = 0.0;

	// Sum the previous layer's outputs (which are inputs)
	// Include the bias node from the previous layer.

	for (unsigned n = 0; n < prevLayer.size(); n++)
	{
		sum += prevLayer[n].getOutputValue() * prevLayer[n].m_outputWeights[m_myIndex].weight;
	}

	m_outputValue = Neuron::transferFunction(sum);
}

void Neuron::calcOutputGradients(float targerValue)
{
	float delta = targerValue - m_outputValue;
	m_gradient = delta * Neuron::transferFunctionDerivative(m_outputValue);
}

void Neuron::calcHiddenGradients(const Layer &nextLayer)
{
	float dow = sumDOW(nextLayer);
	m_gradient = dow * Neuron::transferFunctionDerivative(m_outputValue);
}

void Neuron::updateInputWeights(Layer &prevLayer)
{
	// The weights to updated are in the Connection container
	// in the neurons in the preceding layer
	for (unsigned n = 0; n < prevLayer.size(); n++)
	{
		Neuron &neuron = prevLayer[n];
		float oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

		float newDeltaWeight =
			// Individual input, magnified by the gradient and train rate:
			eta // 0.0==slowlearner; 0.2==medium learner; 1.0==reckless learner
			* neuron.getOutputValue()
			* m_gradient
			// Also add momentum = a fraction of the previous delta weight
			+ alpha // 0.0==no momentum; 0.5==moderate momentum
			* oldDeltaWeight;
		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
	}
}

void Neuron::saveInputWeights(Layer &prevLayer)
{
	// The weights to updated are in the Connection container
	// in the neurons in the preceding layer

	for (unsigned n = 0; n < prevLayer.size(); n++)
	{
		Neuron &neuron = prevLayer[n];
		Weights[NeuronIndex] = neuron.m_outputWeights[m_myIndex].weight;
		NeuronIndex++;
	}

	if(NeuronIndex==Weights.size())
	{
		//save weights from Weights[] to a file
		FIL myFile;

		FRESULT status_open=FR_LOCKED, status_rw=FR_DENIED, status_close=FR_INVALID_OBJECT;
		UINT byteCount=0, byteWritten=0;
		char str_number[30], read_char=' ';
		unsigned index = 0;
		memset(str_number, 0, sizeof(str_number));

		char weight_msg[20], weight_num[6];
		uint8_t str_length=0;
		for (index = 0; index < Weights.size(); index++)
		{
			memset(weight_num, 0, sizeof(weight_num));
			memset(weight_msg, 0, sizeof(weight_msg));
			strcat(weight_msg, "Weight: ");
			itoa(index+1, weight_num, 10);
			strcat(weight_msg, weight_num);
			display_message("Neural Net", "Saving", weight_msg);
			memset(str_number, 0, sizeof(str_number));
			ftoa(Weights[index], str_number, 9);
			strcat(str_number, "\n");
			str_length=strlen(str_number);

			for(uint8_t i=0; i<str_length; i++)
			{
				do
				{
					//WRITE && verify size
					uint32_t file_size=0;
					do
					{
						do
						{
							status_open=f_open(&myFile, "weights.txt", FA_WRITE);
							if(status_open!=FR_OK)
							{
								while(f_mount(&myFAT, SD_Path, 0)!=FR_OK);
								while(f_mount(&myFAT, SD_Path, 1)!=FR_OK);
							}
						}while(status_open!=FR_OK);
						if(myFile.fsize>byteWritten)
						{
							display_message("weights.txt", "please", "ERASE!!!");
							HAL_Delay(3000);
							NeuroBot_sleep();
						}
						while(f_lseek(&myFile, myFile.fsize)!=FR_OK);
						do
						{
							status_rw=f_write(&myFile, &str_number[i], 1, &byteCount);//fprintf(fptr, str_number);
						}while(status_rw!=FR_OK || byteCount!=1);
						do
						{
							status_close=f_close(&myFile);//fclose(fptr);
						}while(status_close!=FR_OK);

						do
						{
							status_open=f_open(&myFile, "weights.txt", FA_READ);
							if(status_open!=FR_OK)
							{
								while(f_mount(&myFAT, SD_Path, 0)!=FR_OK);
								while(f_mount(&myFAT, SD_Path, 1)!=FR_OK);
							}
						}while(status_open!=FR_OK);
						file_size=myFile.fsize;//save file size to a variable
						do
						{
							status_close=f_close(&myFile);//fclose(fptr);
						}while(status_close!=FR_OK);
					}while(file_size!=byteWritten+1);


					//READ
					do
					{
						status_open=f_open(&myFile, "weights.txt", FA_READ);
						if(status_open!=FR_OK)
						{
							while(f_mount(&myFAT, SD_Path, 0)!=FR_OK);
							while(f_mount(&myFAT, SD_Path, 1)!=FR_OK);
						}
					}while(status_open!=FR_OK);
					if(myFile.fsize!=0)
					{
						while(f_lseek(&myFile, myFile.fsize-1)!=FR_OK);
					}
					do
					{
						status_rw=f_read(&myFile, &read_char, 1, &byteCount);//fprintf(fptr, str_number);
					}while(status_rw!=FR_OK || byteCount!=1);
					do
					{
						status_close=f_close(&myFile);//fclose(fptr);
					}while(status_close!=FR_OK);

				}while(str_number[i]!=read_char || (myFile.fsize-1)!=byteWritten);
				byteWritten++;
			}
		}
	}
	HAL_Delay(1500);
}

void Neuron::loadInputWeights(Layer &prevLayer)
{
	// The weights to updated are in the Connection container
	// in the neurons in the preceding layer

	//load weights from a file to Weights[]
	FIL myFile;

	FRESULT status_open=FR_LOCKED, status_rw=FR_DENIED, status_close=FR_INVALID_OBJECT;
	UINT byteCount=0;
	status_open=f_open(&myFile, "weights.txt", FA_READ);
	char str_number[30], c;
	unsigned index = 0, i = 0;
	memset(str_number, 0, sizeof(str_number));

	if (status_open!=FR_OK)
	{
		display_message("weights.txt", "status_open", "ERROR!");//printf("No File!");
		exit(6);
	}

	for (index = 0; index < Weights.size(); index++)
	{
		memset(str_number, 0, sizeof(str_number));
		i = 0;
		while (1)
		{
			status_rw=f_read(&myFile, &c, 1, &byteCount);//c = getc(fptr);
			if(status_rw!=FR_OK)
			{
				display_message("weights.txt", "status_rw", "ERROR!");
				NeuroBot_sleep();
			}
			if (c == '\n')
				break;
			else
			{
				str_number[i++] = c;
			}
		}
		Weights[index] = atof(str_number);
	}

	status_close=f_close(&myFile);//fclose(fptr);
	if(status_close!=FR_OK)
	{
		display_message("weights.txt", "status_close", "ERROR!");
		NeuroBot_sleep();
	}

	for (unsigned n = 0; n < prevLayer.size(); n++)
	{
		Neuron &neuron = prevLayer[n];
		neuron.m_outputWeights[m_myIndex].weight = Weights[NeuronIndex];
		NeuronIndex++;
	}
}

float Neuron::eta = velocity; // overall net learning rate [0.0..1.0]
float Neuron::alpha = momentum; // momentum multiplier of last deltaWeight [0.0..n]

float Neuron::sumDOW(const Layer &nextLayer) const
{
	float sum = 0.0f;

	// Sum our contributions of the errors at the nodes we feed
	for (unsigned n = 0; n < nextLayer.size() - 1; n++)
	{
		sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
	}

	return sum;
}

float Neuron::transferFunction(float x)
{
	// tanh - output range [-1.0..1.0]
	return tanh(x);
}

float Neuron::transferFunctionDerivative(float x)
{
	// tanh derivative
	return 1.0 - pow(tanh(x), 2.0);// approximation return 1.0 - x*x;
}

float Neuron::randomWeight(void)
{
	return rand() / float(RAND_MAX);
}

//************ class NeuralNetwork **************

class NeuralNetwork
{
public:
	NeuralNetwork(const vector<unsigned> &topology);
	void feedForward(const vector<float> &inputValues);
	void backProp(const vector<float> &targetValues);
	void getResults(vector<float> &resultValues) const;
	float getRecentAverageError(void) const { return m_recentAverageError; }
	void saveNeuronWeights();
	void loadNeuronWeights();

private:
	vector<Layer> m_layers; // m_layers[layerNum][neuronNum]
	float m_error;
	float m_recentAverageError;
	float m_recentAverageSmoothingFactor;

};

NeuralNetwork::NeuralNetwork(const vector<unsigned> &topology)
{
	m_error=0;
	m_recentAverageError=0;
	m_recentAverageSmoothingFactor=0;
	unsigned numLayers = topology.size();
	for (unsigned layerNum = 0; layerNum < numLayers; layerNum++)
	{
		m_layers.push_back(Layer());
		unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

		// We have made a new Layer, now fill it with neurons, and add a bias neuron to the layer.
		for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; neuronNum++)
		{
			m_layers.back().push_back(Neuron(numOutputs, neuronNum));
			//cout << "Made a neuron" << endl;
		}

		// Force the bias node's output value to 1.0. It's last neuron created above
		m_layers.back().back().setOutputValue(1.0);
	}
}

void NeuralNetwork::feedForward(const vector<float> &inputValues)
{
	assert(inputValues.size() == m_layers[0].size() - 1);

	// Assign (latch) the input values into the input neurons
	for (unsigned i = 0; i < inputValues.size(); i++)
	{
		m_layers[0][i].setOutputValue(inputValues[i]);
	}

	// Forward propagate
	for (unsigned layerNum = 1; layerNum < m_layers.size(); layerNum++)
	{
		Layer &prevLayer = m_layers[layerNum - 1];
		for (unsigned n = 0; n < m_layers[layerNum].size() - 1; n++)
		{
			m_layers[layerNum][n].feedForward(prevLayer);
		}
	}
}

void NeuralNetwork::backProp(const vector<float> &targetValues)
{
	// Calculate overall net error (RMS of output neuron errors)
	Layer &outputLayer = m_layers.back();
	m_error = 0.0;

	for (unsigned n = 0; n < outputLayer.size() - 1; n++)
	{
		float delta = targetValues[n] - outputLayer[n].getOutputValue();
		m_error += delta * delta;
	}
	m_error /= outputLayer.size() - 1; //get average errorsquared
	m_error = sqrt(m_error); // RMS

							 // Implement a recent average measurement;

	m_recentAverageError =
		(m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
		/ (m_recentAverageSmoothingFactor + 1.0);

	// Calculate output layer gradients
	for (unsigned n = 0; n < outputLayer.size() - 1; n++)
	{
		outputLayer[n].calcOutputGradients(targetValues[n]);
	}

	// Calculate gradients on hidden layers
	for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; layerNum--)
	{
		Layer &hiddenLayer = m_layers[layerNum];
		Layer &nextLayer = m_layers[layerNum + 1];

		for (unsigned n = 0; n < hiddenLayer.size(); n++)
		{
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}

	// For all layers from outputs to first hidden layer.
	// update connection weights

	for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; layerNum--)
	{
		Layer &layer = m_layers[layerNum];
		Layer &prevLayer = m_layers[layerNum - 1];

		for (unsigned n = 0; n < layer.size() - 1; n++)
		{
			layer[n].updateInputWeights(prevLayer);
		}
	}
}

void NeuralNetwork::getResults(vector<float> &resultValues) const
{
	resultValues.clear();

	for (unsigned n = 0; n < m_layers.back().size() - 1; n++)
	{
		resultValues.push_back(m_layers.back()[n].getOutputValue());
	}
}

void NeuralNetwork::saveNeuronWeights()
{
	NeuronIndex = 0;
	// Forward propagate
	for (unsigned layerNum = 1; layerNum < m_layers.size(); layerNum++)
	{
		Layer &prevLayer = m_layers[layerNum - 1];
		for (unsigned n = 0; n < m_layers[layerNum].size() - 1; n++)
		{
			m_layers[layerNum][n].saveInputWeights(prevLayer);
		}
	}
}

void NeuralNetwork::loadNeuronWeights()
{
	NeuronIndex = 0;
	// Forward propagate
	for (unsigned layerNum = 1; layerNum < m_layers.size(); layerNum++)
	{
		Layer &prevLayer = m_layers[layerNum - 1];
		for (unsigned n = 0; n < m_layers[layerNum].size() - 1; n++)
		{
			m_layers[layerNum][n].loadInputWeights(prevLayer);
		}
	}
}

void showVectorValues(string label, vector<float> &v)
{
	cout << label << " ";
	for (unsigned i = 0; i < v.size(); i++)
	{
		cout << v[i] << " ";
	}

	cout << endl;
}

void start_NN()
{
	TrainingData trainData;

	//topology = { InputNodes, HiddenNodes, OutputNodes };
	display_message("Neural Net", "Loading", "......");
	load_topology();
	if (Topology.size() < 3)
	{
		display_message("Topology", "small size", "ERROR!");//cout << endl << "Topology ERROR:\nTopology is too short, may miss some layer.\n" << endl;
		//return -10;
	}
	NeuralNetwork myNet(Topology);

	//load weights from a file...
	if (Weights.size() != get_number_of_weights_from_file())
	{
		display_message("Neural Net", "Training", "Started");
		load_training_data_from_file();

		//cout << endl << "Training started\n" << endl;
		HAL_GPIO_WritePin(LED_Training_GPIO_Port, LED_Training_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_Run_GPIO_Port, LED_Run_Pin, GPIO_PIN_RESET);
		while (1)
		{
			//trainingPass++;
			//cout << endl << "Pass " << trainingPass;

			//Get new input data and feed it forward:
			trainData.getNextInputs(input);
			//showVectorValues(": Inputs:", input);
			myNet.feedForward(input);

			// Train the net what the outputs should have been:
			trainData.getTargetOutputs(target);
			//showVectorValues("Targets: ", target);
			assert(target.size() == Topology.back());
			myNet.backProp(target);//This function alters neurons

								   // Collect the net's actual results:
			myNet.getResults(result);
			//showVectorValues("Outputs: ", result);


			// Report how well the training is working, averaged over recent samples:
			/*cout << "Net recent average error: "
			<< myNet.getRecentAverageError() << endl;*/

			if (myNet.getRecentAverageError() < 0.001)
			{
				display_message("Neural Net", "Saving", "Weights");
				//save weights to a file...
				myNet.saveNeuronWeights();
				break;
			}
		}

		//cout << endl << "Training done\n" << endl;
	}
	else
	{
		display_message("Neural Net", "Loading", "Weights");
		myNet.loadNeuronWeights();
	}

	//cout << endl << "Run mode begin\n" << endl;
	//trainingPass = 0;
	display_message("Neural Net", "Run Mode", "Started");
	HAL_GPIO_WritePin(LED_Training_GPIO_Port, LED_Training_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_Run_GPIO_Port, LED_Run_Pin, GPIO_PIN_SET);
	while (1)
	{
		//trainingPass++;
		//cout << endl << "Run " << trainingPass;
		motor_stop();
		HAL_Delay(500);
		//Get new input data and feed it forward:
		//Make sure that your input data are the same size as InputNodes
		input.clear();
		for (int i = 0; i < InputNodes; i++)
		{
			servoX(1.0f-((float)(i % RunModeQuantization) / (float)(RunModeQuantization - 1)));
			HAL_Delay(300);
			if(i==0)
				HAL_Delay(200);
			input.push_back(measure_distance());
		}
		servoX(0.5f);
		//showVectorValues(": Inputs:", input);
		myNet.feedForward(input);

		// Collect the net's actual results:
		myNet.getResults(result);
		motor_direction(result[0], result[1]);
		//showVectorValues("Outputs: ", result);
		HAL_Delay(200);
		//Sleep(1000);
		if(OK.released || UP.released || DOWN.released || POWER.released)
		{
			break;
		}
	}

	//system("pause");
	motor_stop();
	HAL_GPIO_WritePin(LED_Training_GPIO_Port, LED_Training_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_Run_GPIO_Port, LED_Run_Pin, GPIO_PIN_RESET);
	display_message("Neural Net", "Leaving", "......");
	HAL_Delay(1500);
	buttons_release();
	//return 0;
}

// Created by: Viktor Vano

