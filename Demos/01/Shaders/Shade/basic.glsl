//?#version 450
//?#include "../types.glsl"
//?#include "../AmbientOcclusion/basic.glsl"
//?#include "../Shadow/softshadow.glsl"
//?#include "brdf.glsl"

uniform vec3 gLightColor;
uniform vec3 gLightPos;
uniform float gLightSize;
uniform float gSphereLightStrength;

vec3 norm_2(vec3 p)
{
    vec2 e0 = vec2(0.001, 0);
    vec3 plus = vec3(sdf(p + e0.xyy),sdf(p + e0.yxy),sdf(p + e0.yyx));
    vec3 mins = vec3(sdf(p - e0.xyy),sdf(p - e0.yxy),sdf(p - e0.yyx));
    return normalize(plus - mins);
}

vec3 shadeBasicSphereLight(vec4 Sp, vec3 N, vec3 V, inout Material mat, vec4 Sl)
{
    vec3 toLight = Sl.xyz - Sp.xyz;
	float d = length(toLight);
    vec3 L = toLight/d;
    
    float costheta = Sl.w / d;

    if (dot(N, L) < -costheta)
        return vec3(0);
	
	bool outOfIters;
	float shadowFactor = shadow_soft_new(Sp, Sl, outOfIters);
	
	//vec3 err = float(gShowError && outOfIters) * gShadowIterColor; // TODO errors later
    //mat.diffcol += 2 * err;    mat.ambient +=  0.02 * err;

    //Sample brdf (1 sample)
    vec3 retcol = cook_torrance(V, N, L, mat) * shadowFactor;

    // TODO revise the calcualtions
	float theRealCosTheta = sqrt(1-costheta*costheta);
    return retcol * gLightColor * (0.5 * (1 - theRealCosTheta))*1000/(4*PI*gLightSize*gLightSize)*gSphereLightStrength;
}


vec3 shade(RayCone raycone, float t, Material mat)
{
    vec3 p = RAY(raycone.ray, t);
    vec3 n = norm_2(p);
    vec4 sphere_surf = vec4(p, raycone.tana * t + raycone.rad);
    vec3 retcol = vec3(0);

	// Basic sphere light (not integrating the brdf)
	retcol += shadeBasicSphereLight(sphere_surf, n, -raycone.ray.Direction, mat, vec4(gLightPos, gLightSize));
	
    // Ambient light can only be added once -- for debugging its done at the end
    float occlusion = ambient_occlusion_basic(p, n);
    retcol += mat.ambient * occlusion;
    
    return retcol;
}
