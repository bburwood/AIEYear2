#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include "glm\glm.hpp"

using namespace std;
using namespace glm;

using glm::vec4;

void	MyPrint(int i)
{
	static std::mutex myMutex;
	std::lock_guard<std::mutex>	guard(myMutex);

	printf("%d: Hello Thread!\n", i);
	printf("%d: I'm here!...\n", i);
	printf("%d: ... not there!\n", i);
}



void main()
{
	//	start a separate thread of execution

	std::vector<std::thread>	threads;

	for (unsigned int i = 0; i < 50; ++i)
	{
		threads.push_back(std::thread(MyPrint, i));
	}

	for (unsigned int i = 0; i < 50; ++i)
	{
		//	wait until the thread has ended
		threads[i].join();
	}

	const unsigned int NUM_VECS = 200000;
	vec4*	myVectors = new vec4[NUM_VECS];
	int	numThreads = 25;
	int	chunkLength = NUM_VECS / numThreads;
	for (unsigned int i = 0; i < NUM_VECS; ++i)
	{
		myVectors[i].x = 0.1f + i;
		myVectors[i].y = 0.1f + i * 2;
		myVectors[i].z = 0.1f + i * 3;
		myVectors[i].w = 0.1f + i * 4;
	}
	//std::mutex myMainMutex;
	//	now try it with a lambda function ...	
	std::vector<std::thread>	threads2;
	//std::vector
	for (unsigned int i = 0; i < numThreads; ++i)
	{
		threads2.push_back(std::thread(
			//	defining a lambda thread
			//	lambda can access main scope with &
			[&](int low, int high)
			{
				unsigned int	threadNum = i;
				for (unsigned int j = low; j < high; ++j)
				{
					printf("lambda %d: %d: Before normalise: x: %f y: %f z: %f w: %f\n", threadNum, j, myVectors[j].x, myVectors[j].y, myVectors[j].z, myVectors[j].w);
					myVectors[j] = normalize(myVectors[j]);
					printf("lambda %d: %d: After normalise: x: %f y: %f z: %f w: %f\n", threadNum, j, myVectors[j].x, myVectors[j].y, myVectors[j].z, myVectors[j].w);
				}
				//std::lock_guard<std::mutex>	guard(myMainMutex);
				//printf("Hello lambda Thread!\n");
				//printf("lambda %d: Hello lambda Thread!\n", i);
				//printf("lambda %d: I'm here!...\n", i);
				//printf("lambda %d: ... not there!\n", i);
			}, i * chunkLength, (i + 1) * chunkLength)
		);
	}

	for (unsigned int i = 0; i < numThreads; ++i)
	{
		//	wait until the thread has ended
		threads2[i].join();
	}



	system("pause");
}
