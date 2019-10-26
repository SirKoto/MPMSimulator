#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 2) in vec3 offset;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	vec3 v = (model * vec4(vertex,1.0)).xyz ;

    gl_Position = lightSpaceMatrix * vec4(v + offset, 1.0);
}  