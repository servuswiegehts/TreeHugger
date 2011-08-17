#version 400
//
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inTex;
//
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
//
out vec2 outTex;
//

void main(void)
{	
	outTex = inTex;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * inPosition;
}
