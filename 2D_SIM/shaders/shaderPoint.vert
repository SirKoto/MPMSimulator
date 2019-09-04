#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 colorParticle;

uniform mat4 projection;

out vec3 color;

void main()
{
	color = colorParticle;
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}  