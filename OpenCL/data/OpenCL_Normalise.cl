__kernel void normalize_vec4(__global float4* vectors)
{
	int i = get_global_id(0);
	vectors[i] = normalize( vectors[i] );
}