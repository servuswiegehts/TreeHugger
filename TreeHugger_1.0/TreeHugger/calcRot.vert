#version 400
//
in vec4 inPosition;
in float inRadius;
//
in vec4 inAngle;
in vec4 inAngleVelo;
in vec4 inKiForce;
//
out vec4 passAngle;
out vec4 passAngleVelo;
out vec4 passKiForce;
//
out float passRadius;
//

void main(void)
{	
	passRadius = inRadius;

	passAngle = inAngle;
	passAngleVelo = inAngleVelo;

	gl_Position = inPosition;
}
