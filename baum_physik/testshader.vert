#version 400
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

/*uniform float richtung;*/
uniform float frameCount;
uniform float time;



in vec4 in_Position;
in vec4 in_Color;

out vec4 pass_Color;

void main(void)
{
	float c2 = 0.374571;
	float c4 = 0.129428;

	float x = in_Position.y;
	
	float ux = c2 * x * x + c4 * x * x;
	
	vec4 pos =  modelMatrix * in_Position;

	
	vec4 newPos = pos + ux * sin(2.0*3.14*3.0f * time) * vec4(0.20, 0.0, 0.0, 0.0) ;
	newPos.y = newPos.y-4;
	
	gl_Position = projectionMatrix * viewMatrix * newPos;
	
	pass_Color = in_Color;
}
