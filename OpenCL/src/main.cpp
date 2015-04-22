#include <cstdlib>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include "CL/cl.h"

///	For SSE information go to the Intel Intrinsics Guide
///	mm_add_ps  for adding vector float components ... etc ...
///
///

using namespace std;

struct vec4
{
	float	x, y, z, w;
};

const int	iVEC_COUNT = 37000000;
vec4*	vectors;
LARGE_INTEGER timerfreq;
LARGE_INTEGER timerstart;
LARGE_INTEGER timerend;
double timerdiff;
double clTime;

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
cl_kernel m_kernel;
cl_mem m_buffer;
cl_command_queue m_queue;
cl_event eventID;

bool LoadOpenCLSource(char* a_szFileName, cl_program* a_ProgramID)
{
	FILE* file = fopen("OpenCL_Normalise.cl", "r");
	if (file == nullptr)
	{
		cout << "Failed to open file!\n";
		return false;
	}
	else
	{
		cout << "Successfully opened OpenCl file.\n";
	}
	// read the program source
	fseek(file, 0, SEEK_END);
	unsigned int kernelSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* kernelSource = new char[kernelSize + 1];
	memset(kernelSource, 0, kernelSize + 1);
	fread(kernelSource, sizeof(char), kernelSize, file);
	fclose(file);

	// build program from our source using the device context
	m_program = clCreateProgramWithSource(m_context, 1, (const char**)&kernelSource, &kernelSize, &result);
	if (result != CL_SUCCESS)
		printf("clCreateProgramWithSource failed: %i\n", result);
	result = clBuildProgram(m_program, 1, &m_device, nullptr, nullptr, nullptr);
	if (result != CL_SUCCESS)
		printf("clBuildProgram failed: %i\n", result);

	// extract the kernel
	m_kernel = clCreateKernel(m_program, "normalize_vec4", &result);
	if (result != CL_SUCCESS)
		printf("clCreateKernel failed: %i\n", result);

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
	// create a command queue to process commands
	m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, &result);
	if (result != CL_SUCCESS)
		printf("clCreateCommandQueue failed: %i\n", result);

	if (!LoadOpenCLSource("./src/OpenCL_Normalise.cl", &m_program))
	{
		cout << "Failed to load OpenCL souce!\n";
		return;
	}
}

void OpenCLNormalise()
{
	// create cl buffer for our data and copy it off the host (CPU)
	m_buffer = clCreateBuffer(m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(vec4)* iVEC_COUNT, vectors, &result);
	if (result != CL_SUCCESS)
		printf("clCreateBuffer failed: %i\n", result);

	// set the buffer as the first argument for our kernel
	result = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &m_buffer);
	if (result != CL_SUCCESS)
		printf("clSetKernelArg failed: %i\n", result);

	// process the kernel and give it an event ID
	eventID = 0;
	// we'll give it a size equal to the number of vec4's to process
	size_t globalWorkSize[] = { iVEC_COUNT };
	//	for multi-dimensional data sets use this format:
	//		size_t globalWorkSize[] = { iX_VEC_COUNT, iY_VEC_COUNT, iZ_VEC_COUNT };

	//	kick off the processing
	result = clEnqueueNDRangeKernel(m_queue, m_kernel, 1, nullptr, globalWorkSize, nullptr, 0, nullptr, &eventID);
	if (result != CL_SUCCESS)
		printf("clEnqueueNDRangeKernel failed: %i\n", result);

	// read back the processed data but wait for an event to complete
	result = clEnqueueReadBuffer(m_queue, m_buffer, CL_TRUE, 0, sizeof(vec4)* iVEC_COUNT, vectors, 1, &eventID, nullptr);
	if (result != CL_SUCCESS)
		printf("clEnqueueReadBuffer failed: %i\n", result);

	// finish all opencl commands
	clFlush(m_queue);
	clFinish(m_queue);

	// how long did each event take?
	// get start / end profile data for the event
	cl_ulong clstartTime = 0;
	result = clGetEventProfilingInfo(eventID, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &clstartTime, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetEventProfilingInfo failed: %i\n", result);
	cl_ulong clendTime = 0;
	result = clGetEventProfilingInfo(eventID, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &clendTime, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetEventProfilingInfo failed: %i\n", result);
	// return time is in nanoseconds, so convert to milliseconds
	clTime = (clendTime - clstartTime) * 1.0e-6;
	printf("GPU duration: %fms.\n", clTime);

}

void CleanUpOpenCL()
{
	clReleaseMemObject(m_buffer);
	clReleaseKernel(m_kernel);
	clReleaseProgram(m_program);
	clReleaseCommandQueue(m_queue);
	clReleaseContext(m_context);
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
	QueryPerformanceFrequency(&timerfreq);

	QueryPerformanceCounter(&timerstart);

	CPUNormalise();

	QueryPerformanceCounter(&timerend);
	
	timerdiff = (double)(timerend.QuadPart - timerstart.QuadPart) / (double)timerfreq.QuadPart;

	std::cout << "CPU Normalise Timer: " << timerdiff * 1000.0f << "ms.  " << iVEC_COUNT << " vectors.\n";

	//	now do it on the GPU!!!
	//	re-initialising the vectors
	for (unsigned int i = 0; i < iVEC_COUNT; ++i)
	{
		vectors[i].x = vectors[i].y = vectors[i].z = vectors[i].w = 1.0f;
	}
	SetupOpenCLNormalise();


	OpenCLNormalise();

	cout << "GPU normalise is " << timerdiff / (clTime * 1.0e-3) << " times faster than the CPU normalise!\n";




	CleanUpOpenCL();
	delete[] vectors;
	system("pause");
}
