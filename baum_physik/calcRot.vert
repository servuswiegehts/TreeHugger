#version 400
//
//uniform mat4 projectionMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 modelMatrix;
//
//uniform float frameTime;
//uniform int frameCount;
//
in vec4 in_Position;
in vec4 in_Color;
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
//out vec4 passPosition (=gl_Position)
out vec4 passColor;
out float passRadius;

void main(void)
{	
passRadius = inRadius;

	passColor = in_Color;

	passAngle = inAngle;
	passAngleVelo = inAngleVelo;

	gl_Position = in_Position;
}
