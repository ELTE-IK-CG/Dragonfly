// shadertype=hlsl
#ifndef SHADOW_SOFTSHADOW_SEBBBI_HLSL
#define SHADOW_SOFTSHADOW_SEBBBI_HLSL

#include "..\types.hlsl"
#include "..\SphereTrace\trace.hlsl"

// algorithm from https://www.shadertoy.com/view/lsKcDD
// author: Inigo Quilez, Sebastian Aaltonen
// article: http://iquilezles.org/www/articles/rmshadows/rmshadows.htm

float2 softshadow_Sebbbi( in float3 ro, in float3 rd, in float mint, in float tmax, in float k)
{
	float res = 1.0;
    float t = mint;
	float mintpos = t;
    float ph = 1e10; // big, such that y = 0 on the first iteration
    
    for( int i=0; i<100; i++ )
    {
		float h = sdf( ro + rd*t );

        // traditional technique
        if( !gSebbbi_softShadow )
        {
			float xx = k*h/t;
			if(xx < res){
        		res = xx;
				mint = t;
			}
        }
        // improved technique
        else
        {
            float y = h*h/(2.0*ph);
            float d = sqrt(h*h-y*y);
			float xx = k*d/max(0.0,t-y);
			if(xx < res){
				res = xx;
			}
            ph = h;
        }
        
        t += h;
        
        if( res<0.0001 || t>tmax) break;
        
    }
	float m = clamp( res<0.0001 ? 0.0 : res, 0.0, 1.0 );
    return float2(m, mintpos);
}
#endif