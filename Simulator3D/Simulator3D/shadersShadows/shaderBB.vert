#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec3 normal;

uniform mat4 projectionView;
uniform vec3 colorBBox;
uniform mat4 lightSpaceMatrix;

out vec4 fPosLightSpace;
out vec3 color;
out vec3 o_normal;
out vec3 frag_pos;

void main()
{
	color = abs(normal);
	o_normal = -normal;
	frag_pos = vertex;
	vec4 v = vec4(vertex, 1.0);
	fPosLightSpace = lightSpaceMatrix * v;
    gl_Position = projectionView * v;
}  