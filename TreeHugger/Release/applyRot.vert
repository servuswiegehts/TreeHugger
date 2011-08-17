#version 400
//
in vec4 inPosition;
in float inRadius;
in vec4 inDir;
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
