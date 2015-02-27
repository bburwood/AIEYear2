//	vertex shader
#version 410
layout(location=0) in vec3 position;
layout(location=1) in vec3 velocity;
layout(location=2) in float lifetime;
layout(location=3) in float lifespan;

out vec3 geo_position;
out float geo_lifespan;
out float geo_lifetime;

void	main()
{
	geo_position = position;
	geo_lifespan = lifespan;
	geo_lifetime = lifetime;
}
