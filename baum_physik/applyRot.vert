#version 400
//
in vec4 in_Position;
in vec4 in_Color;
//
out vec4 passColor;
//

void main(void)
{	
	passColor = in_Color;

	gl_Position = in_Position;
}
