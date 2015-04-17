#include <cstdlib>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include "CL/cl.h"

///	For SSE information go to the Intel Intrinsics Guide
///	mm_add_ps  for adding vector float components ... etc ...
///
///

struct vec4
{
	float	x, y, z, w;
};

const int	iVEC_COUNT = 37000000;
vec4*	vectors;

void CPUNormalise()
{
	//	normalise the vectors
	for (unsigned int i = 0; i < iVEC_COUNT; ++i)
	{
		float	d = (vectors[i].x * vectors[i].x +
			vectors[i].y * vectors[i].y +
			vectors[i].z * vectors[i].z +
			vectors[i].w * vectors[i].w);
		float	f1OnLength = 1.0f / sqrtf(d);

		vectors[i].x *= f1OnLength;
		vectors[i].y *= f1OnLength;
		vectors[i].z *= f1OnLength;
		vectors[i].w *= f1OnLength;
	}
}

//	normally these would be stored in the class rather than global
cl_platform_id	m_platform;
cl_device_id	m_device;
cl_int	result;
cl_context m_context;
cl_program m_program;

bool LoadOpenCLSource(char* a_szFileName, cl_program* a_ProgramID)
{
	FILE* file = fopen("myKernelFile.cl", "rb");
	if (file == nullptr)
		return 0;
	// read the program source
	fseek(file, 0, SEEK_END);
	unsigned int kernelSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* kernelSource = new char[kernelSize + 1];
	memset(kernelSource, 0, kernelSize + 1);
	fread(kernelSource, sizeof(char), kernelSize, file);
	fclose(file);

	/////// tutorial page 8 ...


	return true;
}
void SetupOpenCLNormalise()
{
	// get the first platform
	cl_int result = clGetPlatformIDs(1, &m_platform, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetPlatformIDs failed: %i\n", result);
	// get the first device on the platform (default is GPU)
	result = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_DEFAULT, 1, &m_device, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetDeviceIDs failed: %i\n", result);
	// create a context for a device on the specified platform
	cl_context_properties contextProperties[] =
		{ CL_CONTEXT_PLATFORM, (cl_context_properties)m_platform, 0 };
	m_context = clCreateContext(contextProperties, 1, &m_device, nullptr, nullptr, &result);
	if (result != CL_SUCCESS)
		printf("clCreateContext failed: %i\n", result);
	cl_command_queue m_queue;
	// create a command queue to process commands
	m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, &result);
	if (result != CL_SUCCESS)
		printf("clCreateCommandQueue failed: %i\n", result);

	LoadOpenCLSource("./src/OpenCL_Normalise.cl", &m_program);



	clReleaseCommandQueue(m_queue);
	clReleaseContext(m_context);
}

void OpenCLNormalise()
{

}

int	main()
{

	//	allocating the vectors
	vectors = new vec4[iVEC_COUNT];

	//	initialising the vectors
	for (unsigned int i = 0; i < iVEC_COUNT; ++i)
	{
		vectors[i].x = vectors[i].y = vectors[i].z = vectors[i].w = 1.0f;
	}

	//	now set up the timer ...
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);

	CPUNormalise();

	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	
	double diff = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

	std::cout << "CPU Normalise Timer: " << diff * 1000.0f << "ms.  " << iVEC_COUNT << " vectors.'\n'";

	//	now do it on the GPU!!!
	SetupOpenCLNormalise();
	//	re-initialising the vectors
	for (unsigned int i = 0; i < iVEC_COUNT; ++i)
	{
		vectors[i].x = vectors[i].y = vectors[i].z = vectors[i].w = 1.0f;
	}

	QueryPerformanceCounter(&start);

	OpenCLNormalise();

	QueryPerformanceCounter(&end);

	diff = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

	std::cout << "OpenCL Normalise Timer: " << diff * 1000.0f << "ms.  " << iVEC_COUNT << " vectors.'\n'";






	delete[] vectors;
	system("pause");
}
