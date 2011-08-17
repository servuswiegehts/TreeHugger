#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

#ifdef __linux__
#include <GL/glxew.h>
#else
#include <GL/glew.h>
#endif

#include <gl/freeglut.h>

#define IX(i,j) ((i)+(N+2)*(j))
//#define IX3(i,j,k) ( (i) + ( (N+2)*(j) ) + ( (N+2)*(N+2)*(k) ) )
//#define IX3(i,j,k) ( (i) + ( (X+2)*(j) ) + ( (Y+2)*(Z+2)*(k) ) )
#define IX3(i,j,k) ( (i) + ( (X+2)*(j) ) + ( (X+2)*(Y+2)*(k) ) )

//typedef struct
//{
//	float x,y,z;
//	float c1,c2,c3;
//
//} line;


#define	    CL_GL_CONTEXT_KHR			0x2008
#define		CL_EGL_DISPLAY_KHR			0x2009
#define		CL_GLX_DISPLAY_KHR			0x200A
#define		CL_WGL_HDC_KHR				0x200B
#define		CL_CGL_SHAREGROUP_KHR		0x200C

class contextSourceCL{
	public:
		contextSourceCL();
		int initializeCL(/*OpenGL Context has to be created first*/);

		cl_context			getContext();
		cl_device_id*		getDevices();
		cl_command_queue	getCommandQueue();
		cl_program			getProgram();
		cl_kernel			getKernel(int);

	private:
		cl_context          context;
		cl_device_id        *devices;
		cl_command_queue    commandQueue;
		cl_program			program;
		cl_kernel			kernel[16];
		cl_int				status;
	protected:
		
		std::string convertToString(const char *filename);
};
