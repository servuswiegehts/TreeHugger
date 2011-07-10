#version 400
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
//
uniform samplerBuffer thicknessTex;
uniform isamplerBuffer childCountTex;
uniform isamplerBuffer childIDsTex;
uniform isamplerBuffer childIDsPlaceTex;
uniform samplerBuffer kiTex;
uniform samplerBuffer angleTex;
uniform samplerBuffer angleVeloTex;
uniform samplerBuffer posTex;
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

mat4 getRotMat(in vec3 vecc, in float angle)
{
	mat4 rotMat;
	float cosa = cos(angle);
	float sina = sin(angle);

	vec3 vec = normalize(vecc);

	rotMat[0] = vec4(cosa + vec.x * vec.x * (1 - cosa),
					 vec.y * vec.x * (1 - cosa) + vec.z * sina,
					 vec.z * vec.x * (1 - cosa) - vec.y * sina,
					 0.0);
	rotMat[1] = vec4(vec.x * vec.y * (1 - cosa) - vec.z * sina,
					 cosa + vec.y * vec.y * (1 - cosa),
					 vec.z * vec.y * (1 - cosa) + vec.x * sina,
					 0.0);
	rotMat[2] = vec4(vec.x * vec.z * (1 - cosa) + vec.y * sina,
					 vec.y * vec.z * (1 - cosa) - vec.x * sina,
					 cosa + vec.z * vec.z * (1 - cosa),
					 0.0);
	rotMat[3] = vec4(0.0, 0.0, 0.0, 1.0);

	return rotMat;
}

void main(void)
{	
	vec4 posStart = gl_in[0].gl_Position;
	vec4 posEnd = gl_in[1].gl_Position;
	
	//
	//Konstanten
	//
	float l = length(posStart - posEnd); // length of branch
	float m = 45.0; // mass of branch


	vec3 c = 0.5*(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz); // Vector zum Schwerpunkt

	//
	//Windkraft
	//
	float sigma = 0.34; // viscosity coefficient of air
	vec3 v = vec3(0.7, 0.0, 0.0); // Velocity of wind
	float sA = 3.14159 * 1.0 * (passRadius[0] + passRadius[1]) ; // surface area of branch

	vec3 fWind = sA * sigma * v;
	//


	//
	//Restoration force
	//
	float k = 5.37; // rigidity of branch (determined by thickness)
	//vec3 orien = vec3(0.0);
	vec3 newOri = vec3(1.0);

	if (frameCount == 0)
	{
		newOri = (posEnd - posStart).xyz;
		//orien = vec3(0.0);
	}
	else
	{
		newOri = texelFetch(posTex, 2*gl_PrimitiveIDIn+1).xyz -  texelFetch(posTex, 2*gl_PrimitiveIDIn).xyz;
		//orien = gl_in[1].gl_Position.xyz - texelFetch(posTex, 2*gl_PrimitiveIDIn+1).xyz;
		//orien = vec3(0.0) - texelFetch(angleTex, 2*gl_PrimitiveIDIn).xyz;
	}

	vec3 fK = k * ( (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz - newOri);
	//vec3 fK = k * ( orien);
	//


	//
	//Axial damping force
	//
	float my = 3.28; // determined by thickness of branch
	vec3 angleVelo = texelFetch(angleVeloTex, 2*gl_PrimitiveIDIn).xyz;
	vec3 fR = (-1) * my * angleVelo * length(angleVelo);
	if(length(my * dot(angleVelo, angleVelo)) > length(angleVelo))
	{
		fR = (-1) * angleVelo;
	}
	//

	
	//
	//Back-propagation force
	//	
	float kc = 3.0;

	vec3 fT = vec3(0.0);

	int childCount = texelFetch(childCountTex, gl_PrimitiveIDIn).x;
	int childPlace = texelFetch(childIDsPlaceTex, gl_PrimitiveIDIn).x;
	for (int i = 0; i < childCount; ++i)
	{
		int child = texelFetch(childIDsTex, childPlace + i).x;
		float ki = kc * (texelFetch(thicknessTex, child)).x / (texelFetch(thicknessTex, gl_PrimitiveIDIn)).x; // kc fixed propagation coefficient, Th thickness of branch
		fT += ki * texelFetch(kiTex, 2 * child).xyz;
	}
	fT *= (-1);
	//

	//vec3 f = vec3(0.0);
	float ori = 0.0;
	if (length(fWind) < length(fT+fK+fR))
		ori = -1.0;
	else
		ori = 1.0;
	vec3 f = fWind + fT + fK + fR;
	vec3 alpha = cross(c,f)*3/(m*l*l);

	//vec3 passAngle = texelFetch(angleTex, 2*gl_PrimitiveIDIn+1).xyz;
	
	float frameTim = 200.0;
	//TODO: anfangsangle in buffer eintragen
	vec3 outAngleVelo3 = angleVelo + alpha * frameTim/1000;
	//vec3 outAngle3 = passAngle + angleVelo * frameTim/1000 + 0.5 * alpha * (frameTim/1000)*(frameTim/1000);


	outAngle = vec4(0.0);
	outAngleVelo = vec4(outAngleVelo3, 0.0); //passAngleVelo[0] + vec4(angleAcc,1.0) * frameTime/1000;
	//outAngleVelo = vec4(outAngleVelo.xyz, 0.0);
	//outAngle = vec4((outAngle3), 0.0); //passAngle[0] + passAngleVelo[0] * frameTime/1000 + 0.5*vec4(angleAcc,1.0)*(frameTime/1000)*(frameTime/1000);
	//outAngle = vec4(outAngle.xyz, 0.0);
	outKiForce = vec4(fK, 1.0);

	//vec4 posStart = gl_in[0].gl_Position;
	//vec4 posEnd = gl_in[1].gl_Position;

	//int primitiveID = gl_PrimitiveIDIn;
	//float rotAngle = 1.0;
	//if (outAngle.z < 0)
		//rotAngle = length(outAngle.xyz);
	//else
		//rotAngle = (-1) * length(outAngle.xyz);
	mat4 rotMat = getRotMat(outAngleVelo.xyz, length(outAngleVelo.xyz));


	vec4 posRot = rotMat * (posEnd - posStart);
	posStart = gl_in[0].gl_Position;// + vec4(1.0, 0.0, 0.0, 0.0);
	posEnd   = posStart + vec4(posRot.xyz, 0.0);// + vec4(1.0, 0.0, 0.0, 0.0);
	//outAngle = posEnd - posStart;

//
	//int primitiveID = gl_PrimitiveIDIn;
//
	//vec4 posRel = posEnd - posStart;
	////vec3 posCross = cross(posRel.xyz, outAngle.xyz);
	////float cosAngle = dot(normalize(posRel), normalize(outAngle));
	////float angle = acos(cosAngle);
////
	////mat4 rotMat = getRotMat(posCross, angle);
//
	//vec4 posS = posStart;
	//vec4 posE = posEnd;
//
	//vec4 outAngleNorm = normalize(outAngle);
	//float posLength = length(posRel);
//
////	vec4 posRot = rotMat * (posEnd - posStart);
	//posStart = gl_in[0].gl_Position;// + vec4(1.0, 0.0, 0.0, 0.0);
	//posEnd   = posStart + posLength * outAngleNorm;// + vec4(1.0, 0.0, 0.0, 0.0);

	//gl_Position = vec4(length(outAngle.xyz));
	gl_Position = posStart;
	EmitVertex();
	//outAngleVelo = vec4(outAngleVelo3, 0.0); //passAngleVelo[0] + vec4(alpha,1.0) * frameTime/1000;
	////outAngleVelo = vec4(outAngleVelo.xyz, 0.0);
	//outAngle = vec4(outAngle3, 0.0); //passAngle[0] + passAngleVelo[0] * frameTime/1000 + 0.5*vec4(alpha,1.0)*(frameTime/1000)*(frameTime/1000);
	////outAngle = vec4(outAngle.xyz, 0.0);
	//outKiForce = vec4(fK, 1.0);
	//outAngle = posEnd - posStart;
	//gl_Position = vec4(alpha,3.0);
	gl_Position = posEnd;
	EmitVertex();
	EndPrimitive();

}

	////
	////Konstanten
	////
	//float l = 0.20; // length of branch
	//float m = 10.0; // mass of branch
	//float my = 665777; // determined by thickness of branch
	////
	////Windkraft
	////
	//float sigma = 0.00980; // viscosity coefficient of air
	//vec3 v = vec3(253.0, 0.0, 0.0); // Velocity of wind
	////
	////Restoration force
	////
	//float k = 0.000008; // rigidity of branch (determined by thickness)
	////
	////Back-propagation force
	////	
	//float kc = 0.0005;
