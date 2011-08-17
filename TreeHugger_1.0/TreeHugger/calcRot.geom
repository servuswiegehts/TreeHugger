#version 400
//
#define W 62  //Breite
#define H 62  //Höhe
#define D 62  //Tiefe
#define IX3(i,j,k) ( (i) + ( (W+2)*(j) ) + ( (H*2)*(D+2)*(k) ) )
//
layout (lines) in;
layout (line_strip) out;
layout (max_vertices = 2) out;
//
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
//
uniform float frameTime;
uniform int frameCount;
uniform int windCount;
uniform bool windOnOff;
uniform int wurzelID;
//
uniform samplerBuffer thicknessTex;
uniform samplerBuffer posTex;
uniform samplerBuffer windTex;
uniform samplerBuffer angleVeloTex;
uniform isamplerBuffer childCountTex;
uniform isamplerBuffer childIDsTex;
uniform isamplerBuffer childIDsPlaceTex;
uniform samplerBuffer kiTex;

uniform samplerBuffer angleTex;


//
//
in vec4 passColor[];
out vec4 outColor;
//
in vec4 passAngle[];
in vec4 passAngleVelo[];
in vec4 passKiForce[];
//
out vec4 outAngle;
out vec4 outAngleVelo;
out vec4 outKiForce;
//
in float passRadius[];

out vec4 test;



mat4 getRotMat(in vec3 rotAxis, in float angle)
{
	mat4 rotMat;
	float cosa = cos(angle);
	float sina = sin(angle);

	vec3 v = normalize(rotAxis);

	rotMat[0] = vec4(cosa + v.x * v.x * (1 - cosa),
					 v.y * v.x * (1 - cosa) + v.z * sina,
					 v.z * v.x * (1 - cosa) - v.y * sina,
					 0.0);
	rotMat[1] = vec4(v.x * v.y * (1 - cosa) - v.z * sina,
					 cosa + v.y * v.y * (1 - cosa),
					 v.z * v.y * (1 - cosa) + v.x * sina,
					 0.0);
	rotMat[2] = vec4(v.x * v.z * (1 - cosa) + v.y * sina,
					 v.y * v.z * (1 - cosa) - v.x * sina,
					 cosa + v.z * v.z * (1 - cosa),
					 0.0);
	rotMat[3] = vec4(0.0, 0.0, 0.0, 1.0);

	return rotMat;
}



void main(void)
{	

	vec4 posEndNew = vec4(1.0);

	if (windOnOff == true)
	{	
		//
		//Konstanten
		//
		float l = length(gl_in[0].gl_Position - gl_in[1].gl_Position); // length of branch
		float m = 10 * 2* 3.1415 * l * texelFetch(thicknessTex, gl_PrimitiveIDIn).x; // mass of branch


		// Vector zum Schwerpunkt
		vec3 c = vec3(1.0);
		if (frameCount == 0)
		{
			c = 0.5 * ( gl_in[1].gl_Position.xyz -  gl_in[0].gl_Position.xyz );
		}
		else
		{
			c = 0.5 * ( texelFetch(posTex, 2*gl_PrimitiveIDIn+1).xyz - gl_in[0].gl_Position.xyz );
		}
	
		//
		//Windkraft
		//
		vec4 p = projectionMatrix * viewMatrix * modelMatrix * texelFetch(posTex, 2*gl_PrimitiveIDIn+1);
		p = p/p.w;
		int u = int(      p.x /1.0*W+1);
		int v = int((1.0-p.y)/1.0*H+1);
		int w = int((1.0-p.y)/1.0*D+1);
		int wtp = int(IX3(u, v, w));
		// oder (vll)? : int wtp = int(IX3(p.x, p.y, p.z));


		float wind = float(windCount)/25.0;
		//if(wind == 0)
		//	wind = 1.0f;
		float sigma = 1.34; // viscosity coefficient of air
		vec3 windVec = vec3(wind, 0.0, 0.0) + 1.0 * texelFetch(windTex, wtp ).xyz;
	
		float sA = 3.14159 * length(gl_in[1].gl_Position.xyz -  gl_in[0].gl_Position.xyz) * texelFetch(thicknessTex, gl_PrimitiveIDIn).x; // surface area of branch
		vec3 fWind = sA * sigma * windVec;
		//


		//
		//Restoration force
		//
		float k = 67.37; // rigidity of branch (determined by thickness)
		vec3 newOri = vec3(0.0);

		if (frameCount == 0)
		{
			newOri = gl_in[1].gl_Position.xyz;
		}
		else
		{
			newOri = texelFetch(posTex, 2*gl_PrimitiveIDIn+1).xyz;
		}
		vec3 fK = k * (gl_in[1].gl_Position.xyz - newOri);
		//


		//
		//Axial damping force
		//
		float my = 59.28; // determined by thickness of branch
		vec3 angleVelo = texelFetch(angleVeloTex, 2*gl_PrimitiveIDIn).xyz;
		vec3 fR = (-1) * my * angleVelo * length(angleVelo);
		if(length(my * dot(angleVelo, angleVelo)) > dot(angleVelo, vec3(1.0)) )
		{
			fR = (-1) * angleVelo;
		}
		//

	
		//
		//Back-propagation force
		//	
		float kc = 0.025;

		vec3 fT = vec3(0.0);

		int childCount = texelFetch(childCountTex, gl_PrimitiveIDIn).x;
		int childPlace = texelFetch(childIDsPlaceTex, gl_PrimitiveIDIn).x;
		for (int i = 0; i < childCount; ++i)
		{
			int childID = texelFetch(childIDsTex, childPlace + i).x;
			float ki = kc * (texelFetch(thicknessTex, childID)).x / (texelFetch(thicknessTex, gl_PrimitiveIDIn)).x; // kc fixed propagation coefficient, Th thickness of branch
			fT += ki * texelFetch(kiTex, 2 * childID).xyz;
		}
		//fT *= (-1);
		//

	
		vec3 f = fWind + fT + fK + fR ;

		vec3 alpha = cross(c,f)*3/(m*l*l);


		//TODO: anfangsangle in buffer eintragen
		vec3 outAngleVelo3 = angleVelo + alpha *frameTime/1000;


		outAngle = vec4(0.0);
		outAngleVelo = vec4(outAngleVelo3, 0.0);
		outKiForce = vec4(fK, 1.0);


		mat4 rotMat = getRotMat(outAngleVelo.xyz, length(outAngleVelo.xyz));
		vec4 posRot = rotMat * (gl_in[1].gl_Position - gl_in[0].gl_Position);

		posEndNew   = gl_in[0].gl_Position + vec4(posRot.xyz, 0.0);
	}
	else
	{
		posEndNew   = gl_in[1].gl_Position;
	}	

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = posEndNew;
	EmitVertex();


	EndPrimitive();
}