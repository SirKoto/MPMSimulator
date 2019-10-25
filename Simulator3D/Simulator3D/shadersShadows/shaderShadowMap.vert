#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
//layout (location = 1) in vec3 colorParticle;
layout (location = 2) in vec3 offset;
//layout (location = 3) in vec3 aNormal;

uniform mat4 lightSpaceMatrix;

void main()
{
	vec3 v = vertex * 0.005;

    gl_Position = lightSpaceMatrix * vec4(v + offset, 1.0);
}  