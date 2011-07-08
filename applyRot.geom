#version 330
//
layout (lines) in;
layout (triangle_strip) out;
layout (max_vertices = 32) out;
//
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
//
uniform samplerBuffer angleTex;
uniform isamplerBuffer parent;
uniform samplerBuffer posTex;
//
in vec4 passColor[];
out vec4 outColor;
//
out vec4 posStart;
out vec4 posEnd;
out vec4 posRot;
//

void main(void)
{	
	posStart =  texelFetch(posTex, 2*gl_PrimitiveIDIn);
	posEnd =  texelFetch(posTex, 2*gl_PrimitiveIDIn+1);

	int primitiveID = gl_PrimitiveIDIn;

	vec4 posS = posStart;
	vec4 posE = posEnd;

	vec4 posRel = posEnd - posStart;

	posStart = vec4(0.0, 0.0, 0.0, 1.0);
	posEnd = vec4 (posRel.xyz, 1.0);

	int zaehl = 0;
	ivec4 par = ivec4(1);
	while (((texelFetch(parent, primitiveID)).x >= 0))
	{
		par = texelFetch(parent, primitiveID);
		zaehl++;
		primitiveID = par.x;
	}

	primitiveID = gl_PrimitiveIDIn;

	vec4 posStartP = vec4(0.0);
	vec4 posEndP = vec4(0.0);
	vec4 posRelP = vec4(0.0);
	while (((texelFetch(parent, primitiveID)).x >= 0))
	{
		par = texelFetch(parent, primitiveID);
		posStartP = texelFetch(posTex, 2*par.x);
		posEndP = texelFetch(posTex, 2*par.x+1);
		posRelP = posEndP - posStartP;

		posStart += posRelP;
		posEnd += posRelP;

		primitiveID = par.x;
	}


	//posStart = posStart/posStart.w;//vec4(posStart.xyz, 1.0);
	//posStart = vec4(posStart.xyz, 1.0);
	//posEnd = posEnd/posEnd.w;//vec4(posEnd.xyz, 1.0);
	//posEnd = vec4(posEnd.xyz, 1.0);
	
	
	//gl_Position = projectionMatrix * viewMatrix * modelMatrix * posStart;
	//outColor = passColor[0];
	//posRot = texelFetch(posTex, primitiveID*2);
	//EmitVertex();

	//gl_Position = projectionMatrix * viewMatrix * modelMatrix * posEnd;
	//outColor = passColor[1];
	//posRot = texelFetch(posTex, primitiveID*2+1);
	//EmitVertex();

	for (int i = 0; i <= 30; i++)
	{		
		float theta = (2.0 * 3.1415 * float(i)) / 15.0;
		//vec4 posTex = texTexel;// + parentTexel + childCountTexel + childIDsTexel;
		vec4 pos = vec4(0.0);
		pos.x = sin(theta);
		pos.y = 0;
		pos.z = cos(theta);
		
		outColor = passColor[0];
		gl_Position = projectionMatrix * viewMatrix * modelMatrix * (posStart + pos);// + posTex);
		//gl_Position = projectionMatrix * viewMatrix * modelMatrix * (gl_in[0].gl_Position + pos);	
		posRot = posS;
		EmitVertex();


		outColor = passColor[1];
		gl_Position = projectionMatrix * viewMatrix * modelMatrix * (posEnd + pos);// + posTex);
		//gl_Position = projectionMatrix * viewMatrix * modelMatrix * (gl_in[1].gl_Position + pos);
		posRot = posE;
		EmitVertex();
	}
	EndPrimitive();
}