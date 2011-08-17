#include "ocl.h"

ocl::ocl(){};

void ocl::init(){

	cSCL.initializeCL(/*OpenGL Context has to be created first*/);
	context = cSCL.getContext();
	devices = cSCL.getDevices();
	program = cSCL.getProgram();
	commandQueue = cSCL.getCommandQueue();
	jacobi_kernel = cSCL.getKernel(0);
	bnd_kernel = cSCL.getKernel(1);
	advect_kernel = cSCL.getKernel(2);
	swap_kernel = cSCL.getKernel(3);
	jacobi2D_kernel = cSCL.getKernel(4);
	addSourceVelocity_kernel = cSCL.getKernel(5);
	swap2D_kernel = cSCL.getKernel(6);
	bnd2D_kernel = cSCL.getKernel(7);
	project12D_kernel = cSCL.getKernel(8);
	advect2D_kernel = cSCL.getKernel(9);
	project22D_kernel = cSCL.getKernel(10);
	addSourceDensity_kernel = cSCL.getKernel(11);
	bnd_part2_kernel = cSCL.getKernel(12);
	bnd2D_part2_kernel = cSCL.getKernel(13);
	output_kernel = cSCL.getKernel(14);
	output3DTexture_kernel = cSCL.getKernel(15);
};

void ocl::project2D(cl_float2 ac){
	

	//Run project12D for Velocity
	status = clEnqueueNDRangeKernel(commandQueue, project12D_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 jacobi
	status = clSetKernelArg(jacobi_kernel, 0, sizeof(cl_mem),&p);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( jacobi p)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 jacobi
	status = clSetKernelArg(jacobi_kernel, 1, sizeof(cl_mem),&div);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( jacobi div)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 3 jacobi
	status = clSetKernelArg(jacobi_kernel, 3, sizeof(cl_float2),&ac);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( jacobi aux)"<< "("<< status <<")" << std::endl;
	
	//SetKernelArg 0 bnd
	status = clSetKernelArg(bnd_kernel, 0, sizeof(cl_mem),&p);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( bnd project)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 bnd_part2
	status = clSetKernelArg(bnd_part2_kernel, 0, sizeof(cl_mem),&p);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( bnd_part2 project)"<< "("<< status <<")" << std::endl;

	//10 times
	int i=0;
	for(; i < 20; ++i){

		//Run Jacobi Kernel for p
		status = clEnqueueNDRangeKernel(commandQueue, jacobi_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
		if(status != CL_SUCCESS) 
			std::cout<<"Error: clEnqueueNDRangeKernel (project jacobi_kernel)"<< "("<< status <<")" << std::endl;

		//copy aux to velocity for next iteration
		status = clEnqueueCopyBuffer(commandQueue,aux,p,0,0,sizeof(cl_float)* size,0,NULL,NULL);
		if(status != CL_SUCCESS) 
			std::cout<<"Error: clEnqueueNDRangeKernel (project jacobi_kernel)"<< "("<< status <<")" << std::endl;
		
		//Run bnd project
		status = clEnqueueNDRangeKernel(commandQueue, bnd_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
		if(status != CL_SUCCESS) 
			std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_kernel)"<< "("<< status <<")" << std::endl;

		//Run bnd_part2 project
		status = clEnqueueNDRangeKernel(commandQueue, bnd_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
		if(status != CL_SUCCESS) 
			std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_part2_kernel)"<< "("<< status <<")" << std::endl;
	
	}

	//Run project22D Kernel for velocity
	//size_t global[3] = {32,32,32};
	//int tempSize = 32*32*32;

	status = clEnqueueNDRangeKernel(commandQueue, project22D_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( project22D_kernel)"<< "("<< status <<")" << std::endl;

	//Run bnd2D for Velocity
	status = clEnqueueNDRangeKernel(commandQueue, bnd2D_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;
	
	//Run bnd2D_part2 for Velocity
	status = clEnqueueNDRangeKernel(commandQueue, bnd2D_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi1D_kernel)"<< "("<< status <<")" << std::endl;
}

void ocl::addSourceVelocity(cl_float4 *_source, cl_float *_prev_dens,cl_float _diff, cl_float _visc, cl_float _dt, int *_object){


	//get From UI Velocity Source
	status = clEnqueueWriteBuffer(commandQueue,velocitySource,true,0, sizeof(cl_float4) * size, &_source[0],0,NULL,NULL);
	if(status != CL_SUCCESS) 
			std::cout<<"Error: clEnqueueWriteBuffer (velocitySourceBuffer)"<< "("<< status <<")" << std::endl;
	
	////get From UI Density Source
	//status = clEnqueueWriteBuffer(commandQueue,densitySource,true,0, sizeof(cl_float) * size, &_prev_dens[0],0,NULL,NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueWriteBuffer (velocitySourceBuffer)"<< "("<< status <<")" << std::endl;

	////get Obstacle
	//status = clEnqueueWriteBuffer(commandQueue,object,true,0, sizeof(cl_int) * size, &_object[0],0,NULL,NULL);
	//if(status != CL_SUCCESS) 
	//		std::cout<<"Error: clEnqueueWriteBuffer (ObjectBuffer)"<< "("<< status <<")" << std::endl;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////CALCULATE VELOCITIES//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glFlush();

	status = clEnqueueAcquireGLObjects(commandQueue, 1, &output, 0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueAquireGLObjects ( output"<< "("<< status <<")" << std::endl;


	//setKernelArg addSourceVelocity dt
	status = clSetKernelArg(addSourceVelocity_kernel, 2, sizeof(cl_float), &_dt);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( addVelocitySource _dt"<< "("<< status <<")" << std::endl;
	
	size_t global = {size};

	//add Source to Velocity field
	status = clEnqueueNDRangeKernel(commandQueue, addSourceVelocity_kernel, 1, NULL, &global, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( addVelocitySource_kernel)"<< "("<< status <<")" << std::endl;
	

	//swap Buffers
	status = clEnqueueNDRangeKernel(commandQueue, swap2D_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( Copy_kernel)"<< "("<< status <<")" << std::endl;



	cl_float a = _dt  *_visc *X *Y*Z;
	cl_float c = 1+6*a;
	
	if(a!=0.0f)
	{
		cl_float2 ac = {a,c};
		//SetKernelArg 3 jacobi2D
		status = clSetKernelArg(jacobi2D_kernel, 3, sizeof(cl_float2), &ac);
		if(status != CL_SUCCESS) 
			std::cout<<"Error: clSetKernelArg ( jacobi2D ac)"<< "("<< status <<")" << std::endl;
	
		//Run Jacobi2D for Velocity 10 Times
		int i=0;
		for(; i < 20; ++i){

			status = clEnqueueNDRangeKernel(commandQueue, jacobi2D_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
			if(status != CL_SUCCESS) 
				std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;

			//copy temp to velocity for next iteration
			status = clEnqueueCopyBuffer(commandQueue,temp,output,0,0,sizeof(cl_float4) * size,0,NULL,NULL);
			if(status != CL_SUCCESS) 
				std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;
		
			//Run bnd2D for Velocity
			status = clEnqueueNDRangeKernel(commandQueue, bnd2D_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
			if(status != CL_SUCCESS) 
				std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;

			//Run bnd2D_part2 for Velocity
			status = clEnqueueNDRangeKernel(commandQueue, bnd2D_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
			if(status != CL_SUCCESS) 
				std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi1D_kernel)"<< "("<< status <<")" << std::endl;
	
		}
	}
	else{	
			//copy velocitySource to velocity
			status = clEnqueueCopyBuffer(commandQueue,velocitySource,output,0,0,sizeof(cl_float4) * size,0,NULL,NULL);
			if(status != CL_SUCCESS) 
				std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;
		
			//Run bnd2D for Velocity
			status = clEnqueueNDRangeKernel(commandQueue, bnd2D_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
			if(status != CL_SUCCESS) 
				std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;

			//Run bnd2D_part2 for Velocity
			status = clEnqueueNDRangeKernel(commandQueue, bnd2D_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
			if(status != CL_SUCCESS) 
				std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi1D_kernel)"<< "("<< status <<")" << std::endl;
	

	}



	cl_float2 lin_solve = {1,6};
	//run Project2D
	project2D(lin_solve);


	//Swap Velocity - SourceVelocity
	status = clEnqueueNDRangeKernel(commandQueue, swap2D_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( Copy_kernel)"<< "("<< status <<")" << std::endl;

	cl_float4 dt0 = {_dt*X, _dt*Y, _dt*Z};
	
	//SetKernelArg 2 advect2D
	status = clSetKernelArg(advect2D_kernel, 2, sizeof(cl_float4), &dt0);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( advect2D dt)"<< "("<< status <<")" << std::endl;

	//Run advect2D Kernel for Velocity
	status = clEnqueueNDRangeKernel(commandQueue, advect2D_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( advect2D_kernel)"<< "("<< status <<")" << std::endl;
	
	//Run bnd2D for Velocity
	status = clEnqueueNDRangeKernel(commandQueue, bnd2D_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi2D_kernel)"<< "("<< status <<")" << std::endl;

	//Run bnd2D_part2 for Velocity
	status = clEnqueueNDRangeKernel(commandQueue, bnd2D_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueNDRangeKernel ( jacobi1D_kernel)"<< "("<< status <<")" << std::endl;
	

	//Run project2D for Velocity again
	project2D(lin_solve);

	//Run output for Velocity
	//status = clEnqueueNDRangeKernel(commandQueue, output_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel (output_kernel)"<< "("<< status <<")" << std::endl;
	
	status = clEnqueueReleaseGLObjects(commandQueue, 1, &output, 0, NULL, NULL);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clEnqueueReleaseGLObjects ( output)"<< "("<< status <<")" << std::endl;

	//cl_float4 *_el = new cl_float4[size];

	//status = clEnqueueReadBuffer(commandQueue,velocitySource,true,0,sizeof(cl_float4) * size,_el,0,NULL,NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueReadBuffer ( velocity)"<< "("<< status <<")" << std::endl;
	clFlush(commandQueue);

	//for(int i=0; i <= size; ++i){
	//	std::cout<<_el[i].s[0]<<"\n"<<_el[i].s[1]<<"\n"<<_el[i].s[2]<<"\n";
	//}
	
	//status = clEnqueueAcquireGLObjects(commandQueue, 1, &texture, 0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueAquireGLObjects ( texture"<< "("<< status <<")" << std::endl;

	////Run output3dTexture
	//status = clEnqueueNDRangeKernel(commandQueue, output3DTexture_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel (output_kernel)"<< "("<< status <<")" << std::endl;


	//status = clEnqueueReleaseGLObjects(commandQueue, 1, &texture, 0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueReleaseGLObjects ( texture)"<< "("<< status <<")" << std::endl;
	

	//clFinish(commandQueue);
	//TODO: do this by VBO




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////CALCULATE DENSITIES//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////SetKernelArg 2 addDensitySource
	//status = clSetKernelArg(addSourceDensity_kernel, 2, sizeof(cl_float),&_dt);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( addSourceDensity _dt)"<< "("<< status <<")" << std::endl;

	////Run addSourceDensity
	//status = clEnqueueNDRangeKernel(commandQueue, addSourceDensity_kernel, 1, NULL, &global, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel ( addSourceDensity_kernel)"<< "("<< status <<")" << std::endl;
	//
	////Run Swap DensitySource - Density
	//status = clEnqueueNDRangeKernel(commandQueue, swap_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel ( addSourceDensity_kernel)"<< "("<< status <<")" << std::endl;

	////SetKernelArg 0 bnd
	//status = clSetKernelArg(bnd_kernel, 0, sizeof(cl_mem),&density);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( bnd density)"<< "("<< status <<")" << std::endl;

	////SetKernelArg 0 bnd_part2
	//status = clSetKernelArg(bnd_part2_kernel, 0, sizeof(cl_mem),&density);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( bnd_part2 density)"<< "("<< status <<")" << std::endl;
	//
	//cl_float b = _dt  *_diff * X * Y * Z;
	//cl_float d = 1+6*a;
	//
	//if(b!=0)
	//{
	//	cl_float2 bd = {b,d};

	//	//SetKernelArg 0 jacobi
	//	status = clSetKernelArg(jacobi_kernel, 0, sizeof(cl_mem),&density);
	//	if(status != CL_SUCCESS) 
	//		std::cout<<"Error: clSetKernelArg ( jacobi density)"<< "("<< status <<")" << std::endl;
	//	
	//	//SetKernelArg 1 jacobi
	//	status = clSetKernelArg(jacobi_kernel, 1, sizeof(cl_mem),&densitySource);
	//	if(status != CL_SUCCESS) 
	//		std::cout<<"Error: clSetKernelArg ( jacobi densitySource)"<< "("<< status <<")" << std::endl;

	//	//2 is aux
	//	//SetKernelArg 3 jacobi
	//	status = clSetKernelArg(jacobi_kernel, 2, sizeof(cl_float2),&bd);
	//	if(status != CL_SUCCESS) 
	//		std::cout<<"Error: clSetKernelArg ( jacobi bd)"<< "("<< status <<")" << std::endl;


	//	int i=0;
	//	for(;i < 20; ++i)
	//	{
	//		//Run jacobi density
	//		status = clEnqueueNDRangeKernel(commandQueue, jacobi_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	//		if(status != CL_SUCCESS) 
	//			std::cout<<"Error: clEnqueueNDRangeKernel density( jacobi_kernel)"<< "("<< status <<")" << std::endl;
	//
	//		//copy aux to density
	//		status = clEnqueueCopyBuffer(commandQueue,aux,density,0,0,sizeof(cl_float) * size,0,NULL,NULL);
	//		if(status != CL_SUCCESS) 
	//			std::cout<<"Error: clEnqueueCopyBuffer ( density)"<< "("<< status <<")" << std::endl;	

	//		//Run bnd density
	//		status = clEnqueueNDRangeKernel(commandQueue, bnd_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
	//		if(status != CL_SUCCESS) 
	//			std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_kernel)"<< "("<< status <<")" << std::endl;

	//		//Run bnd_part2 density
	//		status = clEnqueueNDRangeKernel(commandQueue, bnd_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
	//		if(status != CL_SUCCESS) 
	//			std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_part2_kernel)"<< "("<< status <<")" << std::endl;
	//	}
	//
	//} 
	//else
	//{
	//	//copy densitySource to density
	//	status = clEnqueueCopyBuffer(commandQueue,densitySource,density,0,0,sizeof(cl_float) * size,0,NULL,NULL);
	//	if(status != CL_SUCCESS) 
	//		std::cout<<"Error: clEnqueueCopyBuffer ( density)"<< "("<< status <<")" << std::endl;

	//	//Run bnd density
	//	status = clEnqueueNDRangeKernel(commandQueue, bnd_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
	//	if(status != CL_SUCCESS) 
	//		std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_kernel)"<< "("<< status <<")" << std::endl;

	//	//Run bnd_part2 density
	//	status = clEnqueueNDRangeKernel(commandQueue, bnd_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
	//	if(status != CL_SUCCESS) 
	//		std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_part2_kernel)"<< "("<< status <<")" << std::endl;
	//}

	////Run Swap DensitySource - Density again
	//status = clEnqueueNDRangeKernel(commandQueue, swap_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel ( addSourceDensity_kernel)"<< "("<< status <<")" << std::endl;
	//
	////SetKernelArg 3 advect
	//status = clSetKernelArg(advect_kernel, 3, sizeof(cl_float4),&dt0);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( advect dt0)"<< "("<< status <<")" << std::endl;

	////Run advect Kernel
	//status = clEnqueueNDRangeKernel(commandQueue, advect_kernel, 3, NULL, globalSize3D, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel ( addVect_kernel)"<< "("<< status <<")" << std::endl;

	////Run bnd density
	//status = clEnqueueNDRangeKernel(commandQueue, bnd_kernel, 2, NULL, globalSize2D, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_kernel)"<< "("<< status <<")" << std::endl;

	////Run bnd_part2 density
	//status = clEnqueueNDRangeKernel(commandQueue, bnd_part2_kernel, 1, NULL, globalSize1D, NULL,0, NULL, NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueNDRangeKernel density( bnd_part2_kernel)"<< "("<< status <<")" << std::endl;
	//

	////TODO: do this by VBO
	//status = clEnqueueReadBuffer(commandQueue,density,true,0,sizeof(cl_float) * size,_dens,0,NULL,NULL);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clEnqueueReadBuffer ( density)"<< "("<< status <<")" << std::endl;



}

void ocl::allocateMemory(GLuint _vbo[1]){

	//CreateBuffer Velocity
	velocity = clCreateBuffer(context, CL_MEM_READ_WRITE   , sizeof(cl_float4) * size, NULL, &status);
    if(status != CL_SUCCESS) 
			std::cout<<"Error: clCreateBuffer (velocityBuffer)"<< "("<< status <<")" << std::endl;

	//CreateBuffer VelocitySource
	velocitySource = clCreateBuffer(context, CL_MEM_READ_WRITE , sizeof(cl_float4) * size,NULL , &status);
    if(status != CL_SUCCESS) 
		std::cout<<"Error: clCreateBuffer (velocitySourceBuffer)"<< "("<< status <<")" << std::endl;

	//CreateBuffer VelocityOutput
	output = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, _vbo[0], &status);
    if(status != CL_SUCCESS) 
		std::cout<<"Error: clCreateBuffer (velocitySourceBuffer)"<< "("<< status <<")" << std::endl;


	//texture = clCreateFromGLTexture3D (context,	CL_WRITE_ONLY, GL_TEXTURE_3D,1,	&_texture[0], status);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clCreateFromGLTexture3D (_texture)"<< "("<< status <<")" << std::endl;

	//CreateBuffer Density
	density = clCreateBuffer(context, CL_MEM_READ_WRITE , sizeof(cl_float) * size, NULL, &status);
    if(status != CL_SUCCESS) 
			std::cout<<"Error: clCreateBuffer (densityBuffer)"<< "("<< status <<")" << std::endl;
	
	//CreateBuffer DensitySource
	densitySource = clCreateBuffer(context, CL_MEM_READ_WRITE , sizeof(cl_float) * size, NULL, &status);
    if(status != CL_SUCCESS) 
			std::cout<<"Error: clCreateBuffer (densitySourceBuffer)"<< "("<< status <<")" << std::endl;

	//CreateBuffer div
	div = clCreateBuffer(context, CL_MEM_READ_WRITE , sizeof(cl_float) * size, NULL, &status);
    if(status != CL_SUCCESS) 
			std::cout<<"Error: clCreateBuffer (divBuffer)"<< "("<< status <<")" << std::endl;

	//CreateBuffer p
	p = clCreateBuffer(context, CL_MEM_READ_WRITE , sizeof(cl_float) * size, NULL, &status);
    if(status != CL_SUCCESS) 
			std::cout<<"Error: clCreateBuffer (pBuffer)"<< "("<< status <<")" << std::endl;

	//CreateBuffer aux
	aux = clCreateBuffer(context, CL_MEM_WRITE_ONLY , sizeof(cl_float) * size, NULL, &status);
    if(status != CL_SUCCESS) 
			std::cout<<"Error: clCreateBuffer (auxBuffer)"<< "("<< status <<")" << std::endl;

	//CreateBuffer temp
	temp = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float4) * size, NULL, &status);
    if(status != CL_SUCCESS) 
		std::cout<<"Error: clCreateBuffer (velocitySourceBuffer)"<< "("<< status <<")" << std::endl;

	//object = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_int) * size, NULL, &status);
 //   if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clCreateBuffer (velocitySourceBuffer)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 addVelocitySource
	status = clSetKernelArg(addSourceVelocity_kernel, 0, sizeof(cl_mem), &velocitySource);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( addVelocitySource velocity)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 addVelocitySource
	status = clSetKernelArg(addSourceVelocity_kernel, 1, sizeof(cl_mem), &output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( addVelocitySource velocitySource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 swap2D
	status = clSetKernelArg(swap2D_kernel, 0, sizeof(cl_mem), &output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( swap2D velocity)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 swap2D
	status = clSetKernelArg(swap2D_kernel, 1, sizeof(cl_mem), &velocitySource);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( swap2D velocitySource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 jacobi2D
	status = clSetKernelArg(jacobi2D_kernel, 0, sizeof(cl_mem), &output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( jacobi2D velocity)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 jacobi2D
	status = clSetKernelArg(jacobi2D_kernel, 1, sizeof(cl_mem), &velocitySource);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( jacobi2D velocitySource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 2 jacobi2D
	status = clSetKernelArg(jacobi2D_kernel, 2, sizeof(cl_mem),&temp);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( jacobi2D temp)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 bnd2D
	status = clSetKernelArg(bnd2D_kernel, 0, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( bnd2D velocity)"<< "("<< status <<")" << std::endl;
	
	////SetKernelArg 1 bnd2D
	//status = clSetKernelArg(bnd2D_kernel, 1, sizeof(cl_mem),&object);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( bnd2D object)"<< "("<< status <<")" << std::endl;

	////SetKernelArg 1 bnd
	//status = clSetKernelArg(bnd_kernel, 1, sizeof(cl_mem),&object);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( bnd object)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 project12D
	status = clSetKernelArg(project12D_kernel, 0, sizeof(cl_mem),&div);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( project12D div)"<< "("<< status <<")" << std::endl;
	
	//SetKernelArg 1 project12D
	status = clSetKernelArg(project12D_kernel, 1, sizeof(cl_mem),&p);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( project12D p)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 2 project12D
	status = clSetKernelArg(project12D_kernel, 2, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( project12D velocity)"<< "("<< status <<")" << std::endl;
	
	//SetKernelArg 2 jacobi
	status = clSetKernelArg(jacobi_kernel, 2, sizeof(cl_mem),&aux);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( jacobi aux)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 projec22D
	status = clSetKernelArg(project22D_kernel, 0, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( project12D velocitySource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 project22D
	status = clSetKernelArg(project22D_kernel, 1, sizeof(cl_mem),&p);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( project12D velocity)"<< "("<< status <<")" << std::endl;
	
	//SetKernelArg 0 advect2D
	status = clSetKernelArg(advect2D_kernel, 0, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( advect2D velocity)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 advect2D
	status = clSetKernelArg(advect2D_kernel, 1, sizeof(cl_mem),&velocitySource);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( advect2D velocitySource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 addDensitySource
	status = clSetKernelArg(addSourceDensity_kernel, 0, sizeof(cl_mem),&densitySource);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( addDensitySource densitySource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 addDensitySource
	status = clSetKernelArg(addSourceDensity_kernel, 1, sizeof(cl_mem),&density);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( addDensitySource density)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 swap
	status = clSetKernelArg(swap_kernel, 0, sizeof(cl_mem),&density);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( swap density)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 swap
	status = clSetKernelArg(swap_kernel, 1, sizeof(cl_mem),&densitySource);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( swap densitSource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 advect
	status = clSetKernelArg(advect_kernel, 0, sizeof(cl_mem),&density);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( advect density)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 advect
	status = clSetKernelArg(advect_kernel, 1, sizeof(cl_mem),&densitySource);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( advect densitySource)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 2 advect
	status = clSetKernelArg(advect_kernel, 2, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( advect densitySource)"<< "("<< status <<")" << std::endl;


	//SetKernelArg 0 bnd2D_part2
	status = clSetKernelArg(bnd2D_part2_kernel, 0, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( advect density)"<< "("<< status <<")" << std::endl;

	cl_float4 xyz = {X,Y,Z};
	//SetKernelArg 1 bnd
	status = clSetKernelArg(bnd_kernel, 1, sizeof(cl_float4),&xyz);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( xyz bnd)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 bnd_part2
	status = clSetKernelArg(bnd_part2_kernel, 1, sizeof(cl_float4),&xyz);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( xyz bnd_part2)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 bnd2D
	status = clSetKernelArg(bnd2D_kernel, 1, sizeof(cl_float4),&xyz);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( xyz bnd2D)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 bnd2D_part2
	status = clSetKernelArg(bnd2D_part2_kernel, 1, sizeof(cl_float4),&xyz);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( xyz bnd2D_part2)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 0 output
	status = clSetKernelArg(output_kernel, 0, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( output velocity)"<< "("<< status <<")" << std::endl;

	//SetKernelArg 1 output
	status = clSetKernelArg(output_kernel, 1, sizeof(cl_mem),&output);
	if(status != CL_SUCCESS) 
		std::cout<<"Error: clSetKernelArg ( output output)"<< "("<< status <<")" << std::endl;

	////SetKernelArg 0 output
	//status = clSetKernelArg(output3DTexture_kernel, 0, sizeof(cl_mem),&velocity);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( output3DTexture velocity)"<< "("<< status <<")" << std::endl;

	////SetKernelArg 0 output
	//status = clSetKernelArg(output3DTexture_kernel, 1, sizeof(cl_mem),&texture);
	//if(status != CL_SUCCESS) 
	//	std::cout<<"Error: clSetKernelArg ( output3DTexture texture)"<< "("<< status <<")" << std::endl;

}

void ocl::setSize(int _size){
	size = _size;
}
void ocl::setN(cl_int3 _N){
	X = _N.s[0];
	Y = _N.s[1];
	Z = _N.s[2];
	setGlobalSize();
}
void ocl::setGlobalSize(){
	
	globalSize3D[0] = (X+2); 
	globalSize3D[1] = (Y+2); 
	globalSize3D[2] = (Z+2);

	globalSize2D[0] = (X+2);
	globalSize2D[1] = (Y+2);

	globalSize1D[0] = (X+2);

}