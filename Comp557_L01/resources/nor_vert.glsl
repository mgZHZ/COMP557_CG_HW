#version 410 core
in vec4 aPos;
in vec3 aNor;
uniform mat4 P;
uniform mat4 MV;
out vec3 vNor;

void main()
{
	gl_Position = P * MV * aPos;
	vNor = (MV * vec4(aNor, 0.0)).xyz;
}
