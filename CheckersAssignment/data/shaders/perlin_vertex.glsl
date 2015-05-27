#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;

out vec3 frag_normal;
out vec3 frag_tangent;
out vec3 frag_bitangent;
out vec2 frag_texcoord;
out vec4 frag_position;

uniform mat4 view_proj;
uniform sampler2D perlin_texture;
uniform float fPerlinScale;
uniform vec2 vMeshDims;
uniform float fHighest;
uniform float fLowest;
uniform float fTerrainHeight;

void main()
{
	vec4 pos = position;
	pos.y = (texture(perlin_texture, texcoord).r - fLowest) * fTerrainHeight / (fHighest - fLowest);	//	should scale the value to the 0-1
	//float fDeltaX = 1.0f / float(vMeshDims.x);
	//float fDeltaZ = 1.0f / float(vMeshDims.y);
	float fDeltaX = 0.0005f;
	float fDeltaZ = 0.0005f;
	vec2 newTexCoord;
	newTexCoord = texcoord;
	newTexCoord.x += fDeltaX;
	vec3 vTangentX;
	vTangentX.x = pos.x + 0.5f;//(fDeltaX * vMeshDims.x);
	vTangentX.y = (texture(perlin_texture, newTexCoord).r - fLowest) * fTerrainHeight / (fHighest - fLowest);
	vTangentX.z = pos.z;
	frag_tangent = vTangentX - pos.xyz;

	newTexCoord = texcoord;
	newTexCoord.y += fDeltaZ;
	vec3 vTangentZ;
	vTangentZ.x = pos.x;
	vTangentZ.y = (texture(perlin_texture, newTexCoord).r - fLowest) * fTerrainHeight / (fHighest - fLowest);
	vTangentZ.z = pos.z + 0.5f;// (fDeltaZ + vMeshDims.y);
	frag_bitangent = vTangentZ - pos.xyz;

	vec3 tangentCross = cross(frag_bitangent, frag_tangent);
	//tangentCross.y = vTangentX.y - vTangentZ.y;
	frag_normal = normalize(tangentCross);
	//frag_normal = normalize(vTangentX.yyy);

	frag_position = pos;
	frag_texcoord = texcoord;
	gl_Position = view_proj * pos;
}
