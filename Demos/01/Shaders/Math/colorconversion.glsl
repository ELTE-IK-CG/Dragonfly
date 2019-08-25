// shadertype=hlsl
#ifndef _COMMON_COLORCONVERSION_HLSL_
#define _COMMON_COLORCONVERSION_HLSL_

// All components are in the range [0..1]
float3 hsv2rgb(float3 c)
{
	c.x = frac(c.x);
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, float3(0.0), float3(1.0)), c.y);
}


#endif