#ifndef WI_VOXEL_CONERACING_HF
#define WI_VOXEL_CONERACING_HF
#include "globals.hlsli"

#ifndef VOXEL_INITIAL_OFFSET
#define VOXEL_INITIAL_OFFSET 2
#endif // VOXEL_INITIAL_OFFSET

// voxels:			3D Texture containing voxel scene with direct diffuse lighting (or direct + secondary indirect bounce)
// P:				world-space position of receiving surface
// N:				world-space normal vector of receiving surface
// coneDirection:	world-space cone direction in the direction to perform the trace
// coneAperture:	tan(coneHalfAngle)
inline float4 ConeTrace(in Texture3D<float4> voxels, in float3 P, in float3 N, in float3 coneDirection, in float coneAperture)
{
	float3 color = 0;
	float alpha = 0;
	
	// We need to offset the cone start position to avoid sampling its own voxel (self-occlusion):
	//	Unfortunately, it will result in disconnection between nearby surfaces :(
	float dist = GetFrame().voxelradiance_size; // offset by cone dir so that first sample of all cones are not the same
	float3 startPos = P + N * GetFrame().voxelradiance_size * VOXEL_INITIAL_OFFSET * SQRT2; // sqrt2 is diagonal voxel half-extent

	// We will break off the loop if the sampling distance is too far for performance reasons:
	while (dist < GetFrame().voxelradiance_max_distance && alpha < 1)
	{
		float diameter = max(GetFrame().voxelradiance_size, 2 * coneAperture * dist);
		float mip = log2(diameter * GetFrame().voxelradiance_size_rcp);

		// Because we do the ray-marching in world space, we need to remap into 3d texture space before sampling:
		//	todo: optimization could be doing ray-marching in texture space
		float3 tc = startPos + coneDirection * dist;
		tc = (tc - GetFrame().voxelradiance_center) * GetFrame().voxelradiance_size_rcp;
		tc *= GetFrame().voxelradiance_resolution_rcp;
		tc = tc * float3(0.5f, -0.5f, 0.5f) + 0.5f;

		// break if the ray exits the voxel grid, or we sample from the last mip:
		if (!is_saturated(tc) || mip >= (float)GetFrame().voxelradiance_mipcount)
			break;

		float4 sam = voxels.SampleLevel(sampler_linear_clamp, tc, mip);

		// this is the correct blending to avoid black-staircase artifact (ray stepped front-to back, so blend front to back):
		float a = 1 - alpha;
		color += a * sam.rgb;
		alpha += a * sam.a;

		// step along ray:
		dist += diameter * GetFrame().voxelradiance_stepsize;
	}

	return float4(color, alpha);
}

// voxels:			3D Texture containing voxel scene with direct diffuse lighting (or direct + secondary indirect bounce)
// P:				world-space position of receiving surface
// N:				world-space normal vector of receiving surface
inline float4 ConeTraceDiffuse(in Texture3D<float4> voxels, in float3 P, in float3 N)
{
	float4 amount = 0;
	float3x3 tangentSpace = get_tangentspace(N);

	for (uint cone = 0; cone < GetFrame().voxelradiance_numcones; ++cone) // quality is between 1 and 16 cones
	{
		float2 hamm = hammersley2d(cone, GetFrame().voxelradiance_numcones);
		float3 hemisphere = hemispherepoint_cos(hamm.x, hamm.y);
		float3 coneDirection = mul(hemisphere, tangentSpace);

		amount += ConeTrace(voxels, P, N, coneDirection, tan(PI * 0.5f * 0.33f));
	}

	// final radiance is average of all the cones radiances
	amount *= GetFrame().voxelradiance_numcones_rcp;
	amount.rgb = max(0, amount.rgb);
	amount.a = saturate(amount.a);

	return amount;
}

// voxels:			3D Texture containing voxel scene with direct diffuse lighting (or direct + secondary indirect bounce)
// P:				world-space position of receiving surface
// N:				world-space normal vector of receiving surface
// V:				world-space view-vector (cameraPosition - P)
inline float4 ConeTraceSpecular(in Texture3D<float4> voxels, in float3 P, in float3 N, in float3 V, in float roughness)
{
	float aperture = tan(roughness * PI * 0.5f * 0.1f);
	float3 coneDirection = reflect(-V, N);

	float4 amount = ConeTrace(voxels, P, N, coneDirection, aperture);
	amount.rgb = max(0, amount.rgb);
	amount.a = saturate(amount.a);

	return amount;
}

#endif
