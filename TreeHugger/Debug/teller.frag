#version 400
//
uniform sampler2D boden;
in vec2 outTex;
//
out vec4 endColor;

void main(void)
{
		endColor= texture2D(boden, outTex);
}