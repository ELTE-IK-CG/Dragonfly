// shadertype=hlsl
#ifndef SHADOW_HARDSHADOW_HLSL
#define SHADOW_HARDSHADOW_HLSL

#include "..\types.hlsl"
#include "..\SphereTrace\trace.hlsl"

float shadow_hard(float3 p, float3 light_pos, SphereTraceDesc params)
{
    float3 diff = light_pos - p;
    float d = length(diff);

    RayDesc ray;
    ray.Origin = p;
    ray.Direction = diff / d;
    ray.Tmin = 0;
    ray.Tmax = d;

    TraceResult tret = DefaultRayTrace(ray, params);

	return float(tret.flags & 1);
}

#endif