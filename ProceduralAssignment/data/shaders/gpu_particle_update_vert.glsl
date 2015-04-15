//	particle update vertex shder
#version 410
layout (location=0) in vec3 position;
layout (location=1) in vec3 velocity;
layout (location=2) in float lifetime;
layout (location=3) in float lifespan;

out vec3 updated_position;
out vec3 updated_velocity;
out float updated_lifetime;
out float updated_lifespan;

uniform float fEmitterAge;
uniform float fEmitterLifespan;
uniform float delta_time;
uniform vec3 emitter_position;
uniform float min_velocity;
uniform float max_velocity;
uniform float min_lifespan;
uniform float max_lifespan;
uniform float gravity_strength;

const float INVERSE_MAX_UINT = 1.0f / 4294967295.0f;

float rand(uint seed, float range)
{
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^ (i >> 12u);
	return float(range * i) * INVERSE_MAX_UINT;
}

void	main()
{
	//	move particles based on velocity
	updated_position = position + velocity * delta_time;
	updated_velocity = velocity + (vec3(0, -gravity_strength * delta_time, 0));
	updated_lifespan = lifespan;

	//	update the lifetime based on delta_time
	updated_lifetime = lifetime +  delta_time;

	//	spawn new particles if we need to
	if ((updated_lifetime > lifespan) && (fEmitterAge < fEmitterLifespan))
	{
		updated_position = emitter_position;
		//	generate random seed
		uint seed = uint(gl_VertexID + (fEmitterAge * 1000));
		//	set its velocity
		float vel_range = max_velocity - min_velocity;
		float vel_len = rand(seed++, vel_range) + min_velocity;

		updated_velocity.x = rand(seed++, 2) - 1;
		updated_velocity.y = rand(seed++, 2) - 1;
		updated_velocity.z = rand(seed++, 2) - 1;
		updated_velocity = normalize(updated_velocity) * vel_len;

		//	set its new lifespan
		updated_lifetime = 0;
		float lifespan_range = max_lifespan - min_lifespan;
		updated_lifespan = rand(seed++, lifespan_range) + min_lifespan;
	}
}
