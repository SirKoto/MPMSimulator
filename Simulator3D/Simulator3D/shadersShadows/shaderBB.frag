#version 330 core
out vec4 out_color;

uniform vec3 camera;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform sampler2D shadowMap;

in vec3 color;
in vec3 o_normal;
in vec3 frag_pos;
in vec4 fPosLightSpace;


float shadowCompute(vec4 fragLightSpace)
{
	vec3 projCoord = fragLightSpace.stp / fragLightSpace.q;
	projCoord = projCoord * 0.5 + 0.5;

	float currDepth = projCoord.z;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x < 2; ++x)
	{
		for(int y = -1; y < 2; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoord.st + vec2(x,y) * texelSize).r;
			shadow += currDepth > pcfDepth ? 0.0 : 1.0;
		}
	}

	return shadow * (1.0/9.0);
}

void main()
{    
	vec3 norm = normalize(o_normal);
	vec3 ldir = normalize(lightPos - frag_pos);

	float diff = max(0, dot(norm, ldir));
	vec3 diff_c = lightColor * diff;

	vec3 viewDir = normalize(camera - frag_pos);
	vec3 ref = reflect(-viewDir, norm);
	float spec = pow(max(0, dot(ref, ldir)), 80) * 0.5;

	vec3 spec_c = spec * lightColor;

	float shadow = shadowCompute(fPosLightSpace);

    out_color = vec4(color * (ambientLight + shadow * (diff_c  + spec_c)), 1.0f);
}  