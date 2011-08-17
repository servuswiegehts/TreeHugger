#version 400
//
layout(location = 0) in vec4 inPosition;
layout(location = 2) in float inRadius;
layout(location = 3) in vec4 inDir;
//
out float passRadius;
out vec4 passDir;
//

void main(void)
{	
	passRadius = inRadius;
	passDir = inDir;	

	gl_Position = inPosition;
}
