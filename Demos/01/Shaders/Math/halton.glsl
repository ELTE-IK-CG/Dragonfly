// shadertype=hlsl
#ifndef _COMMON_HALTON_HLSL_
#define _COMMON_HALTON_HLSL_

#include "../types.hlsl"

// Halton Sequence
float Halton(int index, int base) {
	int i = index;
	float f = 1.0;
	float r = 0.0;
	while (i > 0) {
		f /= base;
		r += f * (i % base);
		i /= base;
	}
	return r;
}

float2 sample2d(int index){
	return float2(Halton(index, 2), Halton(index, 3));	
}


float2 sampleUnitCircle(int index)
{
    float2 ar = sample2d(index) * float2(2*PI,1);
    float2 ret = float2(sin(ar.x), cos(ar.x));
    //sincos(ar.x, ret.x, ret.y);
    return ret * sqrt(ar.y);
}

void setGlobalSeedFromCoords(float2 texCoords){
	int2 pixPos = int2(gResolution*texCoords);
	int indexA = int(pixPos.x) *  173547 + int(pixPos.y) * 364525 + 1013904223;
	indexA = (indexA >> 4) & 0xFFF;
	globalSeed = indexA;	
}

int getSeedFromPosition(float3 p){
	float x = frac(frac(p.z * 363523) + frac(p.x * 362527) + frac(p.y * 364525));
    for (int k = 0; k < 0;++k)
    {
        x = 4 * x * (1 - x);
    }
    return 1024.0 * x;
}

#endif