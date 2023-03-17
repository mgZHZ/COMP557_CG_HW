#version 330 compatibility

in vec3 fragColor;

void main()
{
	gl_FragColor = vec4(fragColor, 1.0);
}
