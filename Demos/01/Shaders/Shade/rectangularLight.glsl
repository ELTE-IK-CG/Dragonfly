// shadertype=hlsl
#ifndef SHADE_RECTANGULARLIGHT_HLSL
#define SHADE_RECTANGULARLIGHT_HLSL

#include "..\types.hlsl"
#include "basic.hlsl"

struct Rectangle{
	float3 center;
	float2 size;
	float3x3 orientation; // ori[0] = normal
};

float4 getSphereForRect(Rectangle rect)
{
	return float4(rect.center, length(rect.size));
}

Rectangle getGlobalLightRect()
{
	Rectangle rect;
	rect.center = gRectLightPos;
	rect.size = gRectLightSize;
	float3 n = gRectLightNorm;
	float alpha = gRectLightAngle;
	const float3 e1 = float3(1, 0, 0);
	const float3 e2 = float3(0, 1, 0);
	const float3 k1 = normalize(cross(n, abs(dot(n, e2)) > 0.9 ? e1 : e2));
	const float3 k2 = normalize(cross(n, k1));
	float sa = sin(alpha), ca = cos(alpha);

	rect.orientation[0] = n;
	rect.orientation[1] = ca*k1 + sa*k2;
	rect.orientation[2] = -sa*k1 + ca*k2;

	return rect;	
}

float rectLightEmission(Rectangle rect, float3 sampleRayDir)
{
	return saturate(dot(-sampleRayDir, rect.orientation[0])); // one way
	//return abs(dot(-sampleRayDir, rect.orientation[0])); // both way
}

bool intersectRect_twoSided(Rectangle rect, RayDesc ray, float tmax)
{
	float dd = dot(rect.orientation[0], ray.Direction);
	//if(dd > 0) return false; // back side
	float t = dot(rect.center-ray.Origin, rect.orientation[0]) / dd;
	if(t < 0 || t > tmax) return false; // behind ray start or ray end
	float3 x = RAY(ray, t);
	return all(abs(mul(rect.orientation, x-rect.center).yz) <= rect.size);
}
bool intersectRect_oneSided(Rectangle rect, RayDesc ray)
{
	float dd = dot(rect.orientation[0], ray.Direction);
	if(dd > 0) return false; // back side
	float t = dot(rect.center-ray.Origin, rect.orientation[0]) / dd;
	if(t < 0) return false; // behind ray start
	float3 x = RAY(ray, t);
	return all(abs(mul(rect.orientation, x-rect.center).yz) <= rect.size);
}
float2 intersectPlane_coordinates(float3 n, float2x3 base, float3 center, RayDesc ray)
{
	float dd = dot(n, ray.Direction);
	float t = dot(center-ray.Origin, n) / dd;
	float3 x = RAY(ray, t);
	return mul(base, x-center);
}

float3 shadeRectangularLight(float4 Sp, float3 N, float3 V, inout Material mat, Rectangle rect)
{
	float4 Sl = getSphereForRect(rect);
    float3 p = Sp.xyz;
    float3 toLight = Sl.xyz - p;
	float d = length(toLight);
    float3 L = toLight/d;

    float costheta = Sl.w / d;

    if (dot(N, L) < -costheta)
        return 0;

    //This is important: we are integrating within this cone:
    RayCone secondary = CreateConeOnSpheres(Sp, Sl);
    
    SphereTraceDesc params; //todo: parameter
        params.maxiters = gShadowIter;
        params.epsilon = 1.0 / float(gShadowLevels);
    float R = secondary.rad;

    TraceResult tf = DefaultMinConeTrace(secondary, params); //NEW!
    //shadowFactor = Default_soft_shadow(Sp, Sl, gShadowIter, gShadowLevels, shadowWasntDone);

	float r_min = clamp(secondary.rad/R, -1, 1);
    // return 0.5 * r_min + 0.5;
    //float3 P_occluder = RAY(secondary.ray, tf.T + gNormEps);
	/** /
	float3 aaa = RAY(secondary.ray, tf.T + gOccluderNormEps*2.0);
	float3 bbb = RAY(secondary.ray, tf.T - gOccluderNormEps*2.0);
	float faaa = sdf(aaa);
	float fbbb = sdf(bbb);
	float3 naaa = norm_2(aaa);
	float3 nbbb = norm_2(bbb);
	float3 N_occluder = normalize(fbbb*naaa+faaa*nbbb);
	/**/
	/** /
	float ee = sdf(RAY(secondary.ray, tf.T))*gOccluderNormEps*10;
    float3 N_occluder = normalize(norm_2(RAY(secondary.ray, tf.T + ee)) + norm_2(RAY(secondary.ray, tf.T - ee)));
	/**/
    float3 N_occluder = normalize(norm_2(RAY(secondary.ray, tf.T + gOccluderNormEps*2.0)) + norm_2(RAY(secondary.ray, tf.T - gOccluderNormEps*2.0)));
	//return N_occluder;

    float3 err = float(gShowError && (tf.flags & 4)) * gShadowIterColor;
    mat.diffcol += 2 * err;    mat.ambient +=  0.02 * err;

    float3 retcol = 0;

    float3 k1 = cross(L, N);
    float2x3 K = float2x3(normalize(k1), normalize(cross(L, k1)));
	float2 n2 = normalize(float2(dot(N_occluder, K[0]), dot(N_occluder, K[1])));
	float2x2 N2 = float2x2(n2, float2(-n2.y, n2.x));
    
    int numOfRays = gBRDFSampleNum;

        //const int seed = floor(abs(100 * p.x + 1000 * p.y * p.z) + floor(abs(10 * (p.y + 4.0) * (1000 * p.z + 1000))) % 13 * 100);
    int seed = getSeedFromPosition(p);//floor(p.z * 364525) + floor(p.x * 173547) + floor(p.y * 472541) + 1013904223;
    //seed = (seed >> 3) & 0xFFFF; // TODO: getseed function(s) to sampling header
    
    RayDesc lightray = secondary.ray; //this will sample the lightsource
	
	float hitcount = 0;

    for (int i = seed; i < seed + numOfRays; ++i)
    {
#ifndef RECT_LIGHT_MODE2
		float2 diskPoint = sampleUnitCircle(i);
        float3 b = Sl.w * mul(diskPoint, K);
		float3 rectPointGlobal = Sl.xyz + b;
        lightray.Direction = normalize(toLight + b);
        
		if(!intersectRect_oneSided(rect, lightray)) continue;
#else
		float2 rectPointLocal = (sample2d(i)*2.0-1.0)*rect.size;
		float3 rectPointGlobal = rect.center + rectPointLocal.x*rect.orientation[1] + rectPointLocal.y*rect.orientation[2];
		lightray.Direction = normalize(rectPointGlobal-lightray.Origin);
		float2 diskPoint = intersectPlane_coordinates(-L, K, rect.center, lightray) / Sl.w;
#endif
	#ifdef HARD_SHADOW_BRDF
		float occlusion = shadow_hard(secondary.ray.Origin, rectPointGlobal, params);
	#else
        //Traditional monte carlo uses MC for the following. In both cases either 0 or 1... should it?
        float occlusion = approximateOcclusion(diskPoint, N2, r_min);
	#endif
        //Light shape and emission is determined here:
        float incomingLight = rectLightEmission(rect, lightray.Direction);
        //Sample brdf
#ifndef RECT_LIGHT_MODE2
        retcol += cook_torrance(V, N, lightray.Direction, mat) * occlusion * incomingLight;
		hitcount += 1;
#else
		float weight = 1; // TODO what is the correct weight to corrigate for the perspective disortion?
        retcol += weight * cook_torrance(V, N, lightray.Direction, mat) * occlusion * incomingLight;
		hitcount += weight;
#endif
    }
	if(hitcount <= 0) return 0;
    // TODO multiply with steradian
	float theRealCosTheta = sqrt(1-costheta*costheta);
    return retcol / hitcount * gLightColor * (0.5 * (1 - theRealCosTheta))*1000/(6*rect.size.x*rect.size.y)*gRectLightStrength;
    //return retcol / hitcount * gLightColor / (0.5 * (1 - costheta));
}





#endif