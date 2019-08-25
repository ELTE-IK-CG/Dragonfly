// shadertype=hlsl
#ifndef _SHADOW_MONTE_CARLO_
#define _SHADOW_MONTE_CARLO_

#include "..\types.hlsl"
#include "..\shadow\hardshadow.hlsl"
#include "..\common\halton.hlsl"

#include "softshadow.hlsl"

float shadow_soft_MonteCarlo(float3 p, float4 sphere1, int numOfRays, out bool didntConverge)
{
	const float3 dir = sphere1.xyz - p;
	const float d = length(dir);
	const float3 v = dir / d;
	const float R = sphere1.w;//(sphere1.w * d) / sqrt(d*d - sphere1.w*sphere1.w);

	SphereTraceDesc params;
	params.epsilon = gMonteCarloEpsilon;
	params.maxiters = 64;

	const float3 e1 = float3(1, 0, 0);
	const float3 e2 = float3(0, 1, 0);
	const float3 k1 = normalize(cross(v, abs(dot(v, e1)) > 0.9 ? e2 : e1));
	const float3 k2 = normalize(cross(v, k1));

	float m = 0.0;

	const int seed = getSeedFromPosition(p);//floor(abs(100 * p.x + 1000 * p.y*p.z ) + floor(abs(10*(p.y+4.0)*(1000*p.z+1000)))%13*100);

	for (int i = seed; i < seed + numOfRays; ++i) {
		float2 rand = sample2d(i);
		float a = rand.x * 2 * PI;
		float r = sqrt(rand.y);
		// Rodrigues' rotation formula:
		// R*k = (cos a)*k + (sin a)*(v x k) + (1-cos a)*v*dot(v,k)
		//  but dot(v,k) == 0
		float3 b = r * R * (cos(a)*k1 + sin(a)*k2);

		m += shadow_hard(p, sphere1.xyz + b, params);
	}

	didntConverge = numOfRays <= 0;
	return didntConverge ? 1.0 : m / (float)numOfRays;
}

float SS_MC_hybrid(float3 p, float4 sphere1, int numOfRays){
	const float3 dir = sphere1.xyz - p;
	const float d = length(dir);
	const float3 v = dir / d;
	const float R = sphere1.w;//(sphere1.w * d) / sqrt(d*d - sphere1.w*sphere1.w);
	
	const float3 e1 = float3(1, 0, 0);
	const float3 e2 = float3(0, 1, 0);
	const float3 k1 = normalize(cross(v, abs(dot(v, e1)) > 0.9 ? e2 : e1));
	const float3 k2 = normalize(cross(v, k1));

	float m = 0.0;

	const int seed = getSeedFromPosition(p);//floor(abs(100 * p.x + 1000 * p.y*p.z ) + floor(abs(10*(p.y+4.0)*(1000*p.z+1000)))%13*100);

	for (int i = seed; i < seed + numOfRays; ++i) {
		float2 rand = sampleUnitCircle(i);
		// Rodrigues' rotation formula:
		// R*k = (cos a)*k + (sin a)*(v x k) + (1-cos a)*v*dot(v,k)
		//  but dot(v,k) == 0
		float3 b = (1-gSSHibridMCSmallSize)*R * (rand.x*k1 + rand.y*k2);
		bool outOfIters;
		m += Default_soft_shadow(float4(p, 0), float4(sphere1.xyz+b, gSSHibridMCSmallSize), gSSHibridMCMaxiter, gSSHibridMCLeves, outOfIters).x;
	}
	
	return numOfRays <= 0 ? 1.0 : m / (float)numOfRays;
}


#endif