//#define X 126
//#define Y 30
//#define Z 126
//#define IX(i,j) ((i)+(N+2)*(j))
//#define IX3(i,j,k) ( (i) + ( (N+2)*(j) ) + ( (N+2)*(N+2)*(k) ) )
//#define IX3(i,j,k) ( (i) + ( (X+2)*(j) ) + ( (Y+2)*(Z+2)*(k) ) )

//#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable
#define IX3(i,j,k) ( (i) + ( (X+2)*(j) ) + ( (X+2)*(Y+2)*(k) ) )
typedef struct
{
	float x,y,z;
	float c1,c2,c3;

} line;
__kernel void jacobi(__global float * x,
					 __global float * x0,
					 __global float * aux,
						const float2 ac)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	
	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;
	

	if(i > 0 && i <= X && j > 0 && j <= Y  && k > 0 && k <= Z)
			aux[IX3(i,j,k)] = (x0[IX3(i,j,k)] + ac.x*(x[IX3(i-1,j,k)] + x[IX3(i+1,j,k)] + x[IX3(i,j-1,k)] + x[IX3(i,j+1,k)] + x[IX3(i,j,k-1)] + x[IX3(i,j,k+1)]))/ac.y;			
		
	//x[IX(0  ,j)] = ac.z == 1 ? -x[IX(1,j)] : x[IX(1,j)];
	//j == 0 ?	 -x[IX(i,1)] : x[IX(i,1)];




}
__kernel void bnd(__global float * x,
				  const float4 xyz)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);

	
	uint X = xyz.x;
	uint Y = xyz.y;
	uint Z = xyz.z;
	
	i += 1;
	j += 1;


	//FLÄCHEN
	if(i <= Y && j <= Z){
			x[IX3(0  ,i,j)] =  x[IX3(1,i,j)];
			x[IX3(X+1,i,j)] =  x[IX3(X,i,j)];
	}

	if(i <= X && j <= Z){
			x[IX3(i,0  ,j)] =  x[IX3(i,1,j)];
			x[IX3(i,Y+1,j)] =  x[IX3(i,Y,j)];
			}

	if(i <= X && j <= Y){
			x[IX3(i,j,0  )] =  x[IX3(i,j,1)];
			x[IX3(i,j,Z+1)] =  x[IX3(i,j,Z)];


	//if(object[IX3(i,j,k)]==1){
	//	if(object[IX3(i,j+1,k)]!=1)
	//		x[IX3(i,j+1,k)] += 0.5f * x[IX3(i,j,k)];
	//	if(object[IX3(i,j-1,k)]!=1)
	//		x[IX3(i,j-1,k)] += 0.5f * x[IX3(i,j,k)];
	//	x[IX3(i,j,k)] = 0.0f;
	//}


	}
}

__kernel void bnd_part2(__global float * x,
						const float4 xyz){

	uint i = get_global_id(0);

	
	uint X = xyz.x;
	uint Y = xyz.y;
	uint Z = xyz.z;
	
	i += 1;

	if(i <= Z){
		//KANTEN
		x[IX3(0  ,0  ,i)] = 0.5f*(x[IX3(1,0  ,i)]+x[IX3(0  ,1,i)]);
		x[IX3(0  ,Y+1,i)] = 0.5f*(x[IX3(1,Y+1,i)]+x[IX3(0  ,Y,i)]);
		x[IX3(X+1,0  ,i)] = 0.5f*(x[IX3(X,0  ,i)]+x[IX3(X+1,1,i)]);
		x[IX3(X+1,Y+1,i)] = 0.5f*(x[IX3(X,Y+1,i)]+x[IX3(X+1,Y,i)]);
	}
	if(i <= X){
		x[IX3(i, 0  ,0  )] = 0.5f*(x[IX3(i, 1,0  )]+x[IX3(i, 0  ,1)]);
		x[IX3(i, 0  ,Z+1)] = 0.5f*(x[IX3(i, 1,Z+1)]+x[IX3(i, 0  ,Z)]);
		x[IX3(i, Y+1,0  )] = 0.5f*(x[IX3(i, Y,0  )]+x[IX3(i, Y+1,1)]);
		x[IX3(i, Y+1,Z+1)] = 0.5f*(x[IX3(i, Y,Z+1)]+x[IX3(i, Y+1,Z)]);
	}
	if(i <= Y){
		x[IX3(0  ,i,0  )] = 0.5f*(x[IX3(1,i,0  )]+x[IX3(0  ,i,1)]);
		x[IX3(0  ,i,Z+1)] = 0.5f*(x[IX3(1,i,Z+1)]+x[IX3(0  ,i,Z)]);
		x[IX3(X+1,i,0  )] = 0.5f*(x[IX3(X,i,0  )]+x[IX3(X+1,i,1)]);
		x[IX3(X+1,i,Z+1)] = 0.5f*(x[IX3(X,i,Z+1)]+x[IX3(X+1,i,Z)]);
	}


	//ECKEN
	if(i == X)
	{
		x[IX3(0  ,0  ,0  )] = 1/3 * (x[IX3(1  ,0  ,0  )] + x[IX3(0   ,1  ,0 )] + x[IX3(0  ,0  ,1  )]);
		x[IX3(X+1,0  ,0  )] = 1/3 * (x[IX3(X  ,0  ,0  )] + x[IX3(X+1 ,1  ,0 )] + x[IX3(X+1,0  ,1  )]);
		x[IX3(0  ,Y+1,0  )] = 1/3 * (x[IX3(1  ,Y+1,0  )] + x[IX3(0  ,Y  ,0  )] + x[IX3(0  ,0  ,Z+1)]);
		x[IX3(0  ,0  ,Z+1)] = 1/3 * (x[IX3(1  ,0  ,Z+1)] + x[IX3(0  ,Y  ,Z+1)] + x[IX3(0  ,0  ,Z  )]);
	}
	if(i == 1){
		x[IX3(X+1,0  ,Z+1)] = 1/3 * (x[IX3(X  ,0  ,Z+1)] + x[IX3(X+1,Y  ,Z+1)] + x[IX3(X+1,0  ,Z  )]);
		x[IX3(X+1,Y+1,0  )] = 1/3 * (x[IX3(X  ,Y+1,0  )] + x[IX3(X+1,Y  ,0  )] + x[IX3(X+1,Y+1,1  )]);
		x[IX3(0  ,Y+1,Z+1)] = 1/3 * (x[IX3(1  ,Y+1,Z+1)] + x[IX3(0  ,Y  ,Z+1)] + x[IX3(0  ,Y+1,Z  )]);
		x[IX3(X+1,Y+1,Z+1)] = 1/3 * (x[IX3(X  ,Y+1,Z+1)] + x[IX3(X+1,Y  ,Z+1)] + x[IX3(X+1,Y+1,Z  )]);
	}
}
__kernel void advect(__global float *d,
					__global float *d0,
					__global float4 *uv,
					const float4 dt0)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	
	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;

	int i0, j0, k0, i1, j1, k1;
	float x, y, z, s0, t0, r0, s1, t1, r1;


	if(i > 0 && i <= X && j > 0 && j <= Y  && k > 0 && k <= Z){

		x = i-dt0.x*uv[IX3(i,j,k)].x;
		y = j-dt0.y*uv[IX3(i,j,k)].y; 
		z = k-dt0.z*uv[IX3(i,j,k)].z;

		if (x<0.5f) x=0.5f; 
		if (x>X+0.5f) x=X+0.5f; 
		
		i0=(int)x; 
		i1=i0+1;

		if (y<0.5f) y=0.5f; 
		if (y>Y+0.5f) y=Y+0.5f; 
		
		j0=(int)y; 
		j1=j0+1;

		if (z<0.5f) z=0.5f; 
		if (z>Z+0.5f) z=Z+0.5f; 
		
		k0=(int)z; 
		k1=k0+1;

		s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1; r1 = z-k0; r0 = 1-r1;	
		
		d[IX3(i,j,k)] = s0 * t0 * r0 * d0[IX3(i0,j0,k0)] +
						s1 * t0 * r0 * d0[IX3(i1,j0,k0)] +
						s0 * t1 * r0 * d0[IX3(i0,j1,k0)] +
						s0 * t0 * r1 * d0[IX3(i0,j0,k1)] +
						s1 * t1 * r0 * d0[IX3(i1,j1,k0)] +
						s0 * t1 * r1 * d0[IX3(i0,j1,k1)] +
						s1 * t0 * r1 * d0[IX3(i1,j0,k1)] +
						s1 * t1 * r1 * d0[IX3(i1,j1,k1)];
	
	
	}
	


}
__kernel void swap(__global float *a,
				  __global float *b)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	
	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;
	

	float temp;
	temp = b[IX3(i,j,k)];

	b[IX3(i,j,k)] = a[IX3(i,j,k)];
	a[IX3(i,j,k)] = temp;
}

__kernel void jacobi2D(__global float4 * x,
					 __global float4 * x0,
					 __global float4 * aux,
						const float2 ac)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;


	if(i > 0 && i <= X && j > 0 && j <= Y  && k > 0 && k <= Z)
		aux[IX3(i,j,k)] = (x0[IX3(i,j,k)] + ac.x*(x[IX3(i-1,j,k)]+x[IX3(i+1,j,k)]+x[IX3(i,j-1,k)]+x[IX3(i,j+1,k)]+x[IX3(i,j,k-1)]+x[IX3(i,j,k+1)]))/ac.y;			


}



__kernel void project12D(__global float * aux,
						 __global float *p,
						 __global float4 * uv)
{	
	
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	
	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;
	


	//teilen durch x,y,z +2
	if(i > 0 && i <= X && j > 0 && j <= Y  && k > 0 && k <= Z)
	{
		aux[IX3(i,j,k)] = -(0.5f) *  ((uv[IX3(i+1,j,k)].x - uv[IX3(i-1,j,k)].x)/X 
									+ (uv[IX3(i,j+1,k)].y - uv[IX3(i,j-1,k)].y)/Y
									+ (uv[IX3(i,j,k+1)].z - uv[IX3(i,j,k-1)].z)/Z
									 );
		
		//aux[IX3(i,j,k)].y = -(0.5f)*(uv[IX3(i+1,j,k)].x - uv[IX3(i-1,j,k)].x + uv[IX3(i,j+1,k)].y - uv[IX3(i,j-1,k)].y
		//				  + uv[IX3(i,j,k+1)].z - uv[IX3(i,j,k-1)].z)/Y;
		//
		//aux[IX3(i,j,k)].z = -(0.5f)*(uv[IX3(i+1,j,k)].x - uv[IX3(i-1,j,k)].x + uv[IX3(i,j+1,k)].y - uv[IX3(i,j-1,k)].y
		//				  + uv[IX3(i,j,k+1)].z - uv[IX3(i,j,k-1)].z)/Z;
		
		p[IX3(i,j,k)] = 0.0f;
	
	
	}

}


__kernel void project22D(__global float4 *velocity,
					  __global float* p)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	
	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;
	
	
	
	if(i > 0 && i <= X && j > 0 && j <= Y  && k > 0 && k <= Z)
	{
	float4 temp;
		temp.x = 0.5f * (float)X * (p[IX3(i+1,j,k)] - p[IX3(i-1,j,k)]);
		temp.y = 0.5f * (float)Y * (p[IX3(i,j+1,k)] - p[IX3(i,j-1,k)]);
		temp.z = 0.5f * (float)Z * (p[IX3(i,j,k+1)] - p[IX3(i,j,k-1)]);

		//temp = normalize(temp);
		velocity[IX3(i,j,k)].x -= temp.x;
		velocity[IX3(i,j,k)].y -= temp.y;
		velocity[IX3(i,j,k)].z -= temp.z;
	//	velocity[IX3(i,j,k)].w = 1.0f;

	}
}

__kernel void bnd2D(__global float4 * x,
				  const float4 xyz)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);

	uint X = xyz.x;
	uint Y = xyz.y;
	uint Z = xyz.z;
	
	i += 1;
	j += 1;

	if(i <= Y && j <= Z)
	{
		x[IX3(0  ,i,j)].x = -x[IX3(1,i,j)].x;
		x[IX3(X+1,i,j)].x = -x[IX3(X,i,j)].x;
	
		x[IX3(0  ,i,j)].y =  x[IX3(1,i,j)].y;
		x[IX3(X+1,i,j)].y =  x[IX3(X,i,j)].y;

		x[IX3(0  ,i,j)].z =  x[IX3(1,i,j)].z;
		x[IX3(X+1,i,j)].z =  x[IX3(X,i,j)].z;
	}
	if(i <= X && j <= Z){
		x[IX3(i,0  ,j)].y =	 -x[IX3(i,1,j)].y;
		x[IX3(i,Y+1,j)].y =  -x[IX3(i,Y,j)].y;
		x[IX3(i,0  ,j)].x =  x[IX3(i,1,j)].x;
		x[IX3(i,Y+1,j)].x =  x[IX3(i,Y,j)].x;
		x[IX3(i,0  ,j)].z =  x[IX3(i,1,j)].z;
		x[IX3(i,Y+1,j)].z =  x[IX3(i,Y,j)].z;
	}
	if(i <= X && j <= Y){
		x[IX3(i,j,0  )].z = -x[IX3(i,j,1)].z;
		x[IX3(i,j,Z+1)].z = -x[IX3(i,j,Z)].z;

		x[IX3(i,j,0  )].x =  x[IX3(i,j,1)].x;
		x[IX3(i,j,Z+1)].x =  x[IX3(i,j,Z)].x;
		x[IX3(i,j,0  )].y =  x[IX3(i,j,1)].y;
		x[IX3(i,j,Z+1)].y =  x[IX3(i,j,Z)].y;
	}
	
}

__kernel void bnd2D_part2(__global float4 *x,
						const float4 xyz){
	uint i = get_global_id(0);

	uint X = xyz.x;
	uint Y = xyz.y;
	uint Z = xyz.z;

	i += 1;

	if(i <= Z)
	{
		x[IX3(0  ,0  ,i)] = 0.5f*(x[IX3(1,0  ,i)]+x[IX3(0  ,1,i)]);
		x[IX3(0  ,Y+1,i)] = 0.5f*(x[IX3(1,Y+1,i)]+x[IX3(0  ,Y,i)]);
		x[IX3(X+1,0  ,i)] = 0.5f*(x[IX3(X,0  ,i)]+x[IX3(X+1,1,i)]);
		x[IX3(X+1,Y+1,i)] = 0.5f*(x[IX3(X,Y+1,i)]+x[IX3(X+1,Y,i)]);
	}
	if(i <= X){
		x[IX3(i, 0  ,0  )] = 0.5f*(x[IX3(i, 1,0  )]+x[IX3(i, 0  ,1)]);
		x[IX3(i, 0  ,Z+1)] = 0.5f*(x[IX3(i, 1,Z+1)]+x[IX3(i, 0  ,Z)]);
		x[IX3(i, Y+1,0  )] = 0.5f*(x[IX3(i, Y,0  )]+x[IX3(i, Y+1,1)]);
		x[IX3(i, Y+1,Z+1)] = 0.5f*(x[IX3(i, Y,Z+1)]+x[IX3(i, Y+1,Z)]);
	}
	if(i <= Y){
		x[IX3(0  ,i,0  )] = 0.5f*(x[IX3(1,i,0  )]+x[IX3(0  ,i,1)]);
		x[IX3(0  ,i,Z+1)] = 0.5f*(x[IX3(1,i,Z+1)]+x[IX3(0  ,i,Z)]);
		x[IX3(X+1,i,0  )] = 0.5f*(x[IX3(X,i,0  )]+x[IX3(X+1,i,1)]);
		x[IX3(X+1,i,Z+1)] = 0.5f*(x[IX3(X,i,Z+1)]+x[IX3(X+1,i,Z)]);
	}

	//ECKEN
	if(i == X)
	{
		x[IX3(0  ,0  ,0  )] = 1/3 * (x[IX3(1  ,0  ,0  )] + x[IX3(0   ,1  ,0 )] + x[IX3(0  ,0  ,1  )]);
		x[IX3(X+1,0  ,0  )] = 1/3 * (x[IX3(X  ,0  ,0  )] + x[IX3(X+1 ,1  ,0 )] + x[IX3(X+1,0  ,1  )]);
		x[IX3(0  ,Y+1,0  )] = 1/3 * (x[IX3(1  ,Y+1,0  )] + x[IX3(0  ,Y  ,0  )] + x[IX3(0  ,0  ,Z+1)]);
		x[IX3(0  ,0  ,Z+1)] = 1/3 * (x[IX3(1  ,0  ,Z+1)] + x[IX3(0  ,Y  ,Z+1)] + x[IX3(0  ,0  ,Z  )]);
	}
	if(i == 1){
		x[IX3(X+1,0  ,Z+1)] = 1/3 * (x[IX3(X  ,0  ,Z+1)] + x[IX3(X+1,Y  ,Z+1)] + x[IX3(X+1,0  ,Z  )]);
		x[IX3(X+1,Y+1,0  )] = 1/3 * (x[IX3(X  ,Y+1,0  )] + x[IX3(X+1,Y  ,0  )] + x[IX3(X+1,Y+1,1  )]);
		x[IX3(0  ,Y+1,Z+1)] = 1/3 * (x[IX3(1  ,Y+1,Z+1)] + x[IX3(0  ,Y  ,Z+1)] + x[IX3(0  ,Y+1,Z  )]);
		x[IX3(X+1,Y+1,Z+1)] = 1/3 * (x[IX3(X  ,Y+1,Z+1)] + x[IX3(X+1,Y  ,Z+1)] + x[IX3(X+1,Y+1,Z  )]);
	}
}

//Trillinear Interpolation
//Semi-Langrangian -->a particle in the cells center traced backwards through the velocity field
__kernel void advect2D(__global float4 *v,
					__global float4 *vs,
					const float4 dt0)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	
	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;


	int i0, j0, k0, i1, j1, k1;
	float x, y, z, s0, t0, r0, s1, t1, r1;


	if(i > 0 && i <= X && j > 0 && j <= Y  && k > 0 && k <= Z){

		x = i-dt0.x*vs[IX3(i,j,k)].x;
		y = j-dt0.y*vs[IX3(i,j,k)].y; 
		z = k-dt0.z*vs[IX3(i,j,k)].z;

		if (x<0.5f) x=0.5f; 
		if (x>X+0.5f) x=X+0.5f; 
		
		i0=(int)x; 
		i1=i0+1;

		if (y<0.5f) y=0.5f; 
		if (y>Y+0.5f) y=Y+0.5f; 
		
		j0=(int)y; 
		j1=j0+1;

		if (z<0.5f) z=0.5f; 
		if (z>Z+0.5f) z=Z+0.5f; 
		
		k0=(int)z; 
		k1=k0+1;

		s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1; r1 = z-k0; r0 = 1-r1;	
		
		v[IX3(i,j,k)] = s0 * t0 * r0 * vs[IX3(i0,j0,k0)] +
						s1 * t0 * r0 * vs[IX3(i1,j0,k0)] +
						s0 * t1 * r0 * vs[IX3(i0,j1,k0)] +
						s0 * t0 * r1 * vs[IX3(i0,j0,k1)] +
						s1 * t1 * r0 * vs[IX3(i1,j1,k0)] +
						s0 * t1 * r1 * vs[IX3(i0,j1,k1)] +
						s1 * t0 * r1 * vs[IX3(i1,j0,k1)] +
						s1 * t1 * r1 * vs[IX3(i1,j1,k1)];
	
	
	}

}


__kernel void swap2D(__global float4 *a,
				  __global float4 *b)
{
	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;
	

	float4 temp;
	temp = b[IX3(i,j,k)];

	b[IX3(i,j,k)] = a[IX3(i,j,k)];
	a[IX3(i,j,k)] = temp;
}

__kernel void addSourceVelocity(__global float4 *source,
								__global float4 *dest,
								const float dt)
{
	uint i = get_global_id(0);

	//int n = 2;
	dest[i] += dt*source[i];


	//dest[0+i*n] += dt*source[0+i*n];
	//dest[1+i*n] += dt*source[1+i*n];
	//dest[2+i*n] += dt*source[2+i*n];
	//dest[3+i*n] += dt*source[3+i*n];
	
	//dest[4+i*n] += dt*source[4+i*n];
	//dest[5+i*n] += dt*source[5+i*n];
	//dest[6+i*n] += dt*source[6+i*n];
	//dest[7+i*n] += dt*source[7+i*n];

}

__kernel void addSourceDensity(__global float *source,
								__global float *dest,
								const float dt)
{
	uint i = get_global_id(0);
	dest[i] += dt*source[i];

}

__kernel void output(__global float4 * vel,
					 __global float4 * out){

	uint i = get_global_id(0);
	uint j = get_global_id(1);
	uint k = get_global_id(2);

	uint X = get_global_size(0)-2;
	uint Y = get_global_size(1)-2;
	uint Z = get_global_size(2)-2;

	float4 temp = vel[IX3(i,j,k)];
	temp = normalize(temp);

	out[IX3(i,j,k)]= temp;

	



	
}
__kernel void output3DTexture(__global float4 * velocity,
							  __write_only image3d_t out)
{
	//vel[IX3(i,j,k)];
	//out[IX3(i,j,k)].w = 0.0f;
	
	//out[0+IX3(i,j,k)*2].c1 =  uvw.x;
	//out[0+IX3(i,j,k)*2].c2 =  uvw.y;
	//out[0+IX3(i,j,k)*2].c3 =  uvw.z;

	//out[1+IX3(i,j,k)*2].x =  xyz.x;
	//out[1+IX3(i,j,k)*2].y =  xyz.y;
	//out[1+IX3(i,j,k)*2].z =  xyz.z;

	//out[1+IX3(i,j,k)*2].c1 = uvw.x;
	//out[1+IX3(i,j,k)*2].c2 = uvw.y;
	//out[1+IX3(i,j,k)*2].c3 = uvw.z;


		//x = (i-0.5f)/S;
	//y = (j-0.5f)/S;
	//z = (k-0.5f)/S;

	//u = (i-0.5f)/X;
	//v = (j-0.5f)/Y;
	//w = (k-0.5f)/Z;

	//uvw.x = u+vel[IX3(i,j,k)].x;
	//uvw.y = v+vel[IX3(i,j,k)].y;
	//uvw.z = w+vel[IX3(i,j,k)].z;
	//
	//xyz.x = x+vel[IX3(i,j,k)].x;
	//xyz.y = y+vel[IX3(i,j,k)].y;
	//xyz.z = z+vel[IX3(i,j,k)].z;


		//uint S, XS,YS,ZS;

	//S = (X >= Z) ? X : Z;
	//S = (S >= Y) ? S : Y;

	//float x,y,z,w,u,v;
	//float4 xyz, uvw;


	//uint i = get_global_id(0);
	//uint j = get_global_id(1);
	//uint k = get_global_id(2);

	//uint X = get_global_size(0)-2;
	//uint Y = get_global_size(1)-2;
	//uint Z = get_global_size(2)-2;
	//
	////sampler_t smplr = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

	//write_imagef(out,(int4)(i,j,k,0),velocity[IX3(i,j,k)]);

}