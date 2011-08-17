#version 400
//
layout (lines) in;
layout (triangle_strip) out;
layout (max_vertices = 80) out;
//
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
//
uniform int wurzelID;
uniform bool leafOnOff;
uniform bool windOnOff;
//
uniform samplerBuffer angleTex;
uniform isamplerBuffer parent;
uniform samplerBuffer posTex;
uniform isamplerBuffer childCountTex;
//
in float passRadius[];
in vec4 passDir[];
//
flat out int blattRinde;
//
out vec2 outTexCoord;
//
//light
out vec4 cameraSpaceVertexNormal;
out vec4 cameraSpacePosition;


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
	int numberPointsInCircle = 8;

	vec4 posStart = vec4(0.0);
	vec4 posEnd = vec4(0.0);

	if (windOnOff == true)
	{
		posStart =  texelFetch(posTex, 2*gl_PrimitiveIDIn);
		posEnd =  texelFetch(posTex, 2*gl_PrimitiveIDIn+1);

		vec4 posRelative = posEnd - posStart;

		posStart = vec4(0.0, 0.0, 0.0, 1.0);
		posEnd = vec4 (posRelative.xyz, 1.0);


		vec4 posStartParent = vec4(0.0);
		vec4 posEndParent = vec4(0.0);
		vec4 posRelativeParent = vec4(0.0);


		int primitiveID = gl_PrimitiveIDIn;
		ivec4 parentID = ivec4(1);

		while ((texelFetch(parent, primitiveID)).x >= 0)
		{
			parentID = texelFetch(parent, primitiveID);
		
			posStartParent = texelFetch(posTex, 2*parentID.x);
			posEndParent = texelFetch(posTex, 2*parentID.x+1);

			posRelativeParent = posEndParent - posStartParent;

			posStart += posRelativeParent;
			posEnd += posRelativeParent;

			primitiveID = parentID.x;
		}
	}
	else
	{
		posStart = gl_in[0].gl_Position;
		posEnd =  gl_in[1].gl_Position;
	}

	blattRinde = 0;  //-> Rinde

	int childCount = texelFetch(childCountTex, gl_PrimitiveIDIn).x;

	float theta = 2.0 * 3.14159 * 1.0 / numberPointsInCircle;

	mat4 rotMat1 = getRotMat(passDir[0].xyz, theta);
	mat4 rotMat2 = getRotMat(passDir[1].xyz, theta);

	mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
	mat4 MV = viewMatrix * modelMatrix;

	for (int i = 0; i < numberPointsInCircle+1; i++)
	{		
		vec4 posSt = vec4(passRadius[0], 0.0, 0.0, 0.0);
		vec4 posEn = vec4(passRadius[1], 0.0, 0.0, 0.0);

		if (childCount == 0)
		{
			posEn.x = 0.01;
		}

		mat4 rot = mat4(1.0);
		for (int j = 0; j < i ; ++j)
		{
			rot *= rotMat1;
		}
		posSt = rot * posSt;

		cameraSpaceVertexNormal = MV * normalize (posSt);

		outTexCoord = vec2(float(i)/float(numberPointsInCircle), 0.0);
		gl_Position = MVP * (posStart + posSt);
		cameraSpacePosition = MV * (posStart + posSt);
		EmitVertex();


		rot = mat4(1.0);
		for (int j = 0; j < i ; ++j)
		{
			rot *= rotMat2;
		}
		posEn = rot * posEn;

		cameraSpaceVertexNormal = viewMatrix*modelMatrix * normalize (posEn);

		outTexCoord = vec2(float(i)/float(numberPointsInCircle), 1.0);
		gl_Position = MVP * (posEnd + posEn);
		cameraSpacePosition = MV * (posEnd + posEn);
		EmitVertex();
	}
	EndPrimitive();


	if (childCount == 0 && leafOnOff == true)
	{
		blattRinde = 1;
		vec4 modelSpacePosition = vec4(0.0);
		mat4 rot = mat4(1.0);
		vec4 pos = vec4(1.0);


		int steps = 3;
		float stepLength = 1.0/float(steps);

		int leafsPerStep = 3;
		float leafAngle = 360.0 / float(leafsPerStep);

		for (int i = 0; i < steps; ++i)
		{
			vec4 leafPos = posStart + (stepLength * i + 0.5 * stepLength) * (posEnd - posStart);

			for (int j = 0; j < leafsPerStep; ++j)
			{
				rot = getRotMat((posEnd - posStart).xyz, (3.1415 * (leafAngle * j + 90 * i)) /180.0);
				cameraSpaceVertexNormal = MV * rot * vec4(0.0, 0.0, 1.0, 0.0); 
				
				outTexCoord = vec2(1.0, 1.0);
				pos = rot * (vec4(-0.25, 0.0, 0.0, 0.0));
				gl_Position = MVP * (pos + leafPos);
				modelSpacePosition = pos + leafPos;
				cameraSpacePosition = MV * modelSpacePosition;
				EmitVertex();

				outTexCoord = vec2(1.0, 0.0);
				pos = rot * (vec4(0.25, 0.0, 0.0, 0.0));
				gl_Position = MVP * (pos + leafPos);
				modelSpacePosition = pos + leafPos;
				cameraSpacePosition = MV * modelSpacePosition;
				EmitVertex();

				outTexCoord = vec2(0.0, 1.0);
				pos = rot * (vec4(-0.25, 0.5, -0.25, 0.0));
				gl_Position = MVP * (pos + leafPos);
				modelSpacePosition = pos + leafPos;
				cameraSpacePosition = MV * modelSpacePosition;
				EmitVertex();

				outTexCoord = vec2(0.0, 0.0);
				pos = rot * (vec4(0.25, 0.5, -0.25, 0.0));
				gl_Position = MVP * (pos + leafPos);
				modelSpacePosition = pos + leafPos;
				cameraSpacePosition = MV * modelSpacePosition;
				EmitVertex();		
			
				EndPrimitive();
			}
		}
	}
}