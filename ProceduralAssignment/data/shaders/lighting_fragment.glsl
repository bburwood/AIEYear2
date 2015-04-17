#version 410

in vec4 lfrag_position;
in vec4 lfrag_normal;
in vec2 lfragTexCoord;

out vec4 lfrag_colour;

uniform sampler2D albedoTexture;
uniform vec3 light_dir;
uniform vec3 light_colour;
//uniform vec3 lmaterial_colour;
uniform vec3 ambient_light;

uniform vec3 eye_pos;
uniform float specular_power;
//uniform float timer;

void main()
{
	vec2 lnewTexCoord = lfragTexCoord;
	//	either the F16 model has reversed texture coords, or I have done something wrong!
	lnewTexCoord.y *= -1.0f;
	vec3 lmaterial_colour = texture(albedoTexture, lnewTexCoord).rgb;
	//vec3 lmaterial_colour = texture(albedoTexture, eye_pos.xz).rgb;

	vec3	lambient = lmaterial_colour * ambient_light;
	//lmaterial_colour = vec3(1.0f, 1.0f, 1.0f);
	vec3	lN = normalize(lfrag_normal.xyz);
	vec3	lL = normalize(light_dir);
	//vec3	lL = normalize(vec3(0.0f, -10.0f, 0.0f));

	float	ld = max(0.0, dot(-lN, -lL));
	vec3 ldiffuse = (vec3(ld) * light_colour * lmaterial_colour);

	vec3	lE = normalize(eye_pos - lfrag_position.xyz);
	vec3	lR = reflect(-lL, -lN);

	float	ls = max(0.0, dot(-lR, lE));
	ls = pow(ls, specular_power);
	//ls = pow(ls, 5.0f);
	vec3	lspecular = vec3(ls) * light_colour * lmaterial_colour;

	lfrag_colour = vec4(lambient + ldiffuse + lspecular, 1);

	//	debugging light components with the following:
	//lfrag_colour = vec4(lambient, 1);
	//lfrag_colour = vec4(ldiffuse, 1);
	//lfrag_colour = vec4(lspecular, 1);
	//lfrag_colour = lfrag_normal;
	//lfrag_colour = lfrag_position;
	//lfrag_colour = vec4(lmaterial_colour, 1);
	//lfrag_colour = vec4(0.0f, lfragTexCoord, 1);
}
