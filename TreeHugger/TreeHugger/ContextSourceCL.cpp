#include "contextSourceCL.h"

contextSourceCL::contextSourceCL(){
	//initializeCL();
};

int contextSourceCL::initializeCL()
{
    status = 0;
    size_t deviceListSize;


	std::cout<<"OCL: Setting up OCL Context...\n";
	cl_platform_id platform;
	status = clGetPlatformIDs(1, &platform, NULL);
	if(status)
	{
		printf("Unable get platform %d\n", status);
		exit(1);
	}
	
    /*
     * Have a look at the available platforms and pick either
     * the AMD one if available or a reasonable default.
     */

   // cl_uint numPlatforms;
//    cl_platform_id platform = NULL;
   // status = clGetPlatformIDs(0, NULL, &numPlatforms);
    //if(status != CL_SUCCESS)
    //{
    //  //  std::cout << "Error: Getting Platforms. (clGetPlatformsIDs)\n";
    //    return 1;
    //}
    
    //if(numPlatforms > 0)
    //{
    //    cl_platform_id* platforms = new cl_platform_id[numPlatforms];
    //    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    //    if(status != CL_SUCCESS)
    //    {
    //    //    std::cout << "Error: Getting Platform Ids. (clGetPlatformsIDs)\n";
    //        return 1;
    //    }
    //    for(unsigned int i=0; i < numPlatforms; ++i)
    //    {
    //        char pbuff[100];
    //        status = clGetPlatformInfo(
    //                    platforms[i],
    //                    CL_PLATFORM_VENDOR,
    //                    sizeof(pbuff),
    //                    pbuff,
    //                    NULL);
    //        if(status != CL_SUCCESS)
    //        {
    //      //      std::cout << "Error: Getting Platform Info.(clGetPlatformInfo)\n";
    //            return 1;
    //        }
    //        platform = platforms[i];
    //        if(!strcmp(pbuff, "Advanced Micro Devices, Inc."))
    //        {
    //            break;
    //        }
    //    }
    //    delete platforms;
    //}

    //if(NULL == platform)
    //{
    ////    std::cout << "NULL platform found so Exiting Application." << std::endl;
    //    return 1;
    //}

    /*
     * If we could find our platform, use it. Otherwise use just available platform.
     */
#ifdef _WIN32
    cl_context_properties cps[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		0};
#else
        cl_context_properties cps[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
                    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
                    0};
#endif
	/////////////////////////////////////////////////////////////////
	// Create an OpenCL context
	/////////////////////////////////////////////////////////////////
    context = clCreateContextFromType(cps, 
                                      CL_DEVICE_TYPE_GPU, 
                                      NULL, 
                                      NULL, 
                                      &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Context. (clCreateContextFromType)\n";
		std::cerr << "ERROR: " << "("<< status <<")" << std::endl;
		system("pause");
		return 1; 
	}

    /* First, get the size of device list data */
    status = clGetContextInfo(context, 
                              CL_CONTEXT_DEVICES, 
                              0, 
                              NULL, 
                              &deviceListSize);
    if(status != CL_SUCCESS) 
	{  
	//	std::cout<<
	//		"Error: Getting Context Info \
	//	    (device list size, clGetContextInfo)\n";
		return 1;
	}
	/////////////////////////////////////////////////////////////////
	// Detect OpenCL devices
	/////////////////////////////////////////////////////////////////
    devices = (cl_device_id *)malloc(deviceListSize);
	if(devices == 0)
	{
		std::cout<<"Error: No devices found.\n";
		return 1;
	}
	    /* Now, get the device list data */
    status = clGetContextInfo(
			     context, 
                 CL_CONTEXT_DEVICES, 
                 deviceListSize, 
                 devices, 
                 NULL);
    if(status != CL_SUCCESS) 
	{ 
		std::cout<<
			"Error: Getting Context Info \
		    (device list, clGetContextInfo)\n";
		return 1;
	}

	/////////////////////////////////////////////////////////////////
	// Create an OpenCL command queue
	/////////////////////////////////////////////////////////////////
    commandQueue = clCreateCommandQueue(
					   context, 
                       devices[0], 
                       0, 
                       &status);
    if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Creating Command Queue. (clCreateCommandQueue)\n";
		return 1;
	}
		/////////////////////////////////////////////////////////////////
	// Load CL file, build CL program object, create CL kernel object
	/////////////////////////////////////////////////////////////////
    const char * filename  = "solver.cl";
    std::string  sourceStr = convertToString(filename);
    const char * source    = sourceStr.c_str();
    size_t sourceSize[]    = { strlen(source) };

    program = clCreateProgramWithSource(
			      context, 
                  1, 
                  &source,
				  sourceSize,
                  &status);
	if(status != CL_SUCCESS) 
	{ 
	  std::cout<<
			   "Error: Loading Binary into cl_program \
			   (clCreateProgramWithBinary)\n";
	  return 1;
	}

    /* create a cl program executable for all the devices specified */
    status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
    if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Error: Building Program (clBuildProgram)\n";;

		printf("Unable build program %d\n", status);
		char log[10000];
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 10000, log, NULL);
		printf("BUILD LOG:\n%s", log);
		system("pause");
		return 1; 
	}

    /* get a kernel object handle for a kernel with the given name */
    kernel[0] = clCreateKernel(program, "jacobi", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}

	kernel[1] = clCreateKernel(program, "bnd", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[2] = clCreateKernel(program, "advect", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[3] = clCreateKernel(program, "swap", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[4] = clCreateKernel(program, "jacobi2D", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[5] = clCreateKernel(program, "addSourceVelocity", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[6] = clCreateKernel(program, "swap2D", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[7] = clCreateKernel(program, "bnd2D", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[8] = clCreateKernel(program, "project12D", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}
	kernel[9] = clCreateKernel(program, "advect2D", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}	
	kernel[10] = clCreateKernel(program, "project22D", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (clCreateKernel)\n";
		return 1;
	}	
	kernel[11] = clCreateKernel(program, "addSourceDensity", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (addSourceDensity)\n";
		return 1;
	}
	kernel[12] = clCreateKernel(program, "bnd_part2", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (bnd_part2)\n";
		return 1;
	}
	kernel[13] = clCreateKernel(program, "bnd2D_part2", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (bnd2D_part2)\n";
		return 1;
	}
	
	kernel[14] = clCreateKernel(program, "output", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (output)\n";
		return 1;
	}
	kernel[15] = clCreateKernel(program, "output3DTexture", &status);
    if(status != CL_SUCCESS) 
	{  
		std::cout<<"Error: Creating Kernel from program. (output)\n";
		return 1;
	}

	std::cout<<"OCL: Finished\n\n";
	return 0;
}

cl_context contextSourceCL::getContext(){
	return context;
}

cl_device_id* contextSourceCL::getDevices(){
	return devices;
}

cl_command_queue contextSourceCL::getCommandQueue(){
	return commandQueue;
}

cl_program contextSourceCL::getProgram(){
	return program;
}

cl_kernel contextSourceCL::getKernel(int i){
	return kernel[i];
}

std::string contextSourceCL::convertToString(const char *filename)
{
	size_t size;
	char*  str;
	std::string s;

	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);

		str = new char[size+1];
		if(!str)
		{
			f.close();
			return NULL;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
	
		s = str;
		delete[] str;
		return s;
	}
	else
	{
		std::cout << "\nFile containg the kernel code(\".cl\") not found. Please copy the required file in the folder containg the executable.\n";
		exit(1);
	}
	return NULL;
}
