// shadertype=hlsl
#ifndef SHADOW_SOFTSHADOW_CSABA_HLSL
#define SHADOW_SOFTSHADOW_CSABA_HLSL

#include "..\types.hlsl"

float2 softshadow_Csaba(float4 sphere0, float4 sphere1)
{
	float dist = distance(sphere0.xyz, sphere1.xyz);
	if (gSSCsabaStepCount <= 0) return float2(1,0);
	
	float3 l = normalize(sphere1.xyz - sphere0.xyz);
	
	float tana = (sphere1.w - sphere0.w)/dist; // almost, good enough for now
	
	const RayDesc r2  = {sphere0.xyz, 0, l, dist-sphere1.w};
	float t = r2.Tmin;
	float m = 1;
	for(int i = 0; i < gSSCsabaStepCount; ++i)
	{
		if(t >= r2.Tmax) return float2(m, t);
		float rt = t*tana + sphere0.w;
		float ft = sdf(RAY(r2,t), r2.Direction).x + rt;
		float newm = smoothstep(0,1,0.5*ft/rt);
		if(newm < m)
		{
			if(newm < gSSCsabaEpsilon) return float2(newm,t);
			m = newm;
			t -= ft*gSSCsabaStepMultiplier*0.5; //step back to find smaller values
		}
		t += ft*gSSCsabaStepMultiplier;
	}
	return float2(m,t);
}
#endif