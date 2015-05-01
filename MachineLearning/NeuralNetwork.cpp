#include "matrix.h"
#include "NeuralNetwork.h"
#include "Agent.h"
#include <math.h>      
#include <iostream>
using namespace std;
extern int gScreenWidth;
extern int gScreenHeight;

//colours used for displaying the classifications that the neural network as learnt
const glm::vec4 gColours[] = { glm::vec4(0, 0, 0, 0.5), glm::vec4(1, 0, 0, 0.5), glm::vec4(0, 1, 0, 0.5) };

//Constructor for a neural network
NeuralNetwork::NeuralNetwork(int numberInputs, int numberHidden, int numberOutputs)
{
	//keep local copies of the network dimensions
	_numberInputs = numberInputs;
	_numberHidden = numberHidden;
	_numberOutputs = numberOutputs;
	//create hidden layer matrix
	hiddenLayer = new Matrix(_numberInputs, _numberHidden);
	hiddenLayer->random(); //fills matrix with random floats between 0 and 1
	*hiddenLayer = *hiddenLayer - 0.5f; //range is now -.5 to .5
	*hiddenLayer = *hiddenLayer * 2; //range is now -1 to 1
}

NeuralNetwork::~NeuralNetwork()
{
	delete(hiddenLayer);
	delete(outputLayer);
}

//sigmoidal function
float activationFunction(float x)
{
	return 1.0f / (1.0f + exp(-x));
}

//applies a function to all the elements in an array
void NeuralNetwork::applyActivationFunction(Matrix* matrix)
{
	for (int i = 0; i < matrix->dim1(); i++)
	{
		for (int j = 0; j < matrix->dim2(); j++)
		{
			(*matrix)(i, j) = activationFunction((float)(*matrix)(i, j));
		}
	}
}

void NeuralNetwork::trainNetwork(vector<glm::vec3>& data)
{
	//first we need to convert out history vector into a form we can work with.  Annoying but necessary..
	Matrix input(data.size(), _numberInputs); //we need to normalize these values
	Matrix targetOutput(data.size(), _numberOutputs);
	//we need a matrix which just has the input values in it and one which just has the output values
	for (unsigned int i = 0; i < data.size(); i++)
	{
		input(i, 0) = data[i].x;
		input(i, 1) = data[i].y;
		//now we need to convert our training classification into an output node index.
		//a training value of 0 means the node with index 1 is set to 1 and all others are 0, 
		//a value of 1 means that output index 1 is 1 and all others are 0, etc...
		for (int index = 0; index < _numberOutputs; index++)
		{
			if (data[i].z == index)  //if this is the index of our training data then set this node to 1
			{
				targetOutput(i, index) = 1;
			}
			else //else set it to zero
			{
				targetOutput(i, index) = 0;
			}
		}
	}
	//feed the data forward through the input layers
	Matrix hiddenOutput = feedforward(&input);
	Matrix output1 = pseudoinverse(hiddenOutput) * targetOutput;
	// if we have already trained the network delete the old output layer
	if (outputLayer != NULL)
	{
		delete(outputLayer);
	}
	outputLayer = new Matrix(output1); //and add the trained outputlayer
}

void NeuralNetwork::setUpDebugRender(int numberOfSamples)
{
	_numberOfSamples = numberOfSamples;
	for (float j = 0; j < 1; j += 1 / (float)numberOfSamples)
	{
		for (float i = 0; i < 1; i += 1 / (float)numberOfSamples)
		{
			float value = 0;
			testData.push_back(glm::vec3(i,j,value));
		}
	}
}

void NeuralNetwork::renderDebug(glm::vec2 screenOffset, int displaySize, vector<glm::vec3>& memory)
{
	testNetwork(testData);
	float elementPixelSize = displaySize / (float)_numberOfSamples;
	glm::vec2 extent = glm::vec2(elementPixelSize/2, elementPixelSize/2);
	for (auto element:testData)
	{

		int colourIndex = (int)element.z; //classification is returned in Z.
		colourIndex = glm::clamp(colourIndex, 0, 3);
		glm::vec4 colour = gColours[colourIndex];
		glm::vec2 screenPos(element.x  * displaySize, element.y *displaySize );
		Gizmos::add2DAABBFilled(screenOffset + screenPos, extent, colour);
	}
	for (auto element : memory)
	{

		glm::vec4 colour = (int)element.z % 2 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 0);
		glm::vec2 screenPos(element.x  * displaySize, element.y *displaySize);
		Gizmos::add2DCircle(screenOffset + screenPos, elementPixelSize/2.0f, 10, colour);
	}
}

//feeds inputs to the neural network through the hidden layer.
Matrix NeuralNetwork::feedforward(Matrix* input)
{
	Matrix hiddenOutput = *input * *hiddenLayer;
	applyActivationFunction(&hiddenOutput);
	return hiddenOutput;
}

//runs one pair of x,y coordinates through the trained network and returns the classied value in the z component
void NeuralNetwork::testNetwork(glm::vec3& data)
{
	Matrix* input = new Matrix(1, 2); //we need to normalize these values
	(*input)(0, 0) = data.x;
	(*input)(0, 1) = data.y;
	Matrix hiddenOutput = feedforward(input);
	Matrix output = hiddenOutput * *outputLayer;
	data.z = (float)classify(output.get_row(0));
	delete(input);
}



//runs a set of x,y coordinates through the trained network and returns the classied values for each in the z component
void NeuralNetwork::testNetwork(vector<glm::vec3>& data)
{
	//first we need to convert out history vector into a form we can work with.  Annoying but necessary..
	Matrix* input = new Matrix(data.size(), 2); //we need to normalize these values
	//we need a matrix which just has the input values in it and one which just has the output values
	int rows = input->dim1();
	for (int i = 0; i < rows; i++)
	{
		(*input)(i, 0) = data[i].x;
		(*input)(i, 1) = data[i].y;
	}
	//feed the data forward through our input layer
	Matrix hiddenOutput = feedforward(input);
	//finally we "test" our output layer
	Matrix output = hiddenOutput * *outputLayer;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		data[i].z = (float)classify(output.get_row(i));
	}
	delete(input);
}

int NeuralNetwork::classify(Matrix& data)
{
	//very simple classifier, returns the index of the output node which has the highest value
	int bestIndex = 0;
	for (int index = 1; index < data.dim2(); index++)
	{
		if (data(0, index) > data(0, bestIndex))
		{
			bestIndex = index;
		}
	}
	return bestIndex;
}