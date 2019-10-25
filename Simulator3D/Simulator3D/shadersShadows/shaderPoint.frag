#version 330 core
out vec4 out_color;

in vec3 color;
in vec3 fpos;
in vec3 normal;
in vec4 fposLightSpace;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;


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
	vec3 norm = normal;
	vec3 ligthDir = normalize(fpos - lightPos);

	float diff = max(0,dot(-ligthDir, norm));

	vec3 diffLight = lightColor * diff;
	
    float shadow = shadowCompute(fposLightSpace);

    out_color = vec4(color * (ambientLight + shadow * diffLight), 1.0f);  
}