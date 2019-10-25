#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 colorParticle;
layout (location = 2) in vec3 offset;
layout (location = 3) in vec3 aNormal;

uniform mat4 projectionView;
uniform mat4 lightSpaceMatrix;

out vec3 color;
out vec3 fpos;
out vec3 normal;
out vec4 fposLightSpace;

void main()
{
	vec3 v = vertex * 0.005;
	
	color = colorParticle;
	normal = aNormal;
	vec4 finalPos = vec4(v + offset, 1.0);
	fpos = finalPos.xyz;
	fposLightSpace = lightSpaceMatrix * finalPos;

    gl_Position = projectionView * finalPos;
}  