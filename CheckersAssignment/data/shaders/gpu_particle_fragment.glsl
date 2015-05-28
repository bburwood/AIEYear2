//	fragment shader
#version 410
in vec4 frag_color;
in vec2 fragTexCoord;

out vec4 out_color;

uniform sampler2D particleTexture;
uniform float fEmitterAge;
uniform float fEmitterLifespan;
uniform float fEmitterAgePercent;	//	why do I have to calculate this in C++ and pass it in, rather than calculate it in the shader!?!?

void	main()
{
	vec4 vTextureColour = texture(particleTexture, fragTexCoord);
	float fColourIntensity = (vTextureColour.r + vTextureColour.g + vTextureColour.b) / 3.0f;
	vec3 finalColour = frag_color.xyz * fColourIntensity;
	//finalColour.b = 1.0f;
	float fAlphaValue;
	//fAlphaValue =  float(max(0.03f, ((fEmitterAge / fEmitterLifespan)) ));	//	WHY could I NOT use the alpha value calculated here properly?!?!??!
	fAlphaValue = max(0.0f, clamp(1.0f - fEmitterAgePercent, 0.0f, 1.0f)) * fColourIntensity;

//	out_color.a = fColourIntensity * (1.0f - (fEmitterAge / fEmitterLifespan));
//	out_color = vec4(0.0f, 0.0f, 0.0f, fAlphaValue);	//	also add in reducing alpha based on lifetime
	out_color = vec4(float(finalColour.r), float(finalColour.g), float(finalColour.b), float(fAlphaValue));
}
