#include <CL/cl.h>
#include "contextSourceCL.h"




class ocl{
	public:
		ocl();
		void init();

		void allocateMemory(GLuint vbo[1]);
		void addSourceVelocity(cl_float4 * source, cl_float *prev_dens, cl_float diff, cl_float visc, cl_float dt, int *object);
		
		void setSize(int size);
		void setN(cl_int3 N);

		

	private:
		contextSourceCL		cSCL;
		cl_context          context;
		cl_device_id        *devices;
		cl_command_queue    commandQueue;
		cl_program			program;
		cl_kernel			jacobi_kernel;
		cl_kernel			project_kernel;
		cl_kernel			project2_kernel;
		cl_kernel			bnd_kernel;
		cl_kernel			advect_kernel;
		cl_kernel			swap_kernel;
		cl_kernel			jacobi2D_kernel;
		cl_kernel			addSourceVelocity_kernel;
		cl_kernel			addSourceDensity_kernel;
		cl_kernel			swap2D_kernel;
		cl_kernel			bnd2D_kernel;
		cl_kernel			project12D_kernel;
		cl_kernel			project22D_kernel;
		cl_kernel			advect2D_kernel;
		cl_kernel			bnd_part2_kernel;
		cl_kernel			bnd2D_part2_kernel;
		cl_kernel			output_kernel;
		cl_kernel			output3DTexture_kernel;

		cl_mem				x;
		cl_mem				x0;
		cl_mem				u;
		cl_mem				v;
		cl_mem				p;
		cl_mem				u0;
		cl_mem				v0;
		cl_mem				aux;
		cl_mem				div;
		cl_mem				bnd_x;
		cl_mem				d;
		cl_mem				d0;
		cl_mem				object;
		cl_mem				velocity;
		cl_mem				velocitySource;
		cl_mem				temp;
		cl_mem				density;
		cl_mem				densitySource;
		cl_mem				output;
		cl_mem				texture;

		cl_int				status;
		size_t				globalSize3D[3];
		size_t				globalSize2D[2];
		size_t				globalSize1D[1];
		cl_int				size;
		cl_int				X;
		cl_int				Y;
		cl_int				Z;

	protected:
		void setGlobalSize();
		void project2D(cl_float2 ac);


};
