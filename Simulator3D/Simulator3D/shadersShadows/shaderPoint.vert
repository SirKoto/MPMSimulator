#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec3 colorParticle;
layout (location = 2) in vec3 offset;
layout (location = 3) in vec3 aNormal;

uniform mat4 projectionView;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec3 color;
out vec3 fpos;
out vec3 normal;
out vec4 fposLightSpace;

void main()
{
	vec4 v = model * vec4(vertex, 1.0);
	
	color = colorParticle;
	normal = aNormal;
	v = v + vec4(offset, 0.0);
	fpos = v.xyz;
	fposLightSpace = lightSpaceMatrix * v;

    gl_Position = projectionView * v;
}  