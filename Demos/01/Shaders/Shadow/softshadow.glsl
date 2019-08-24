//?#version 450

//?#include "..\types.glsl"
//?#include "..\sdf.glsl"
//?#include "..\SphereTrace\step.glsl"


uniform int gShadowMaxiter;
uniform float gShadowEpsilon;

#define ONE_OVER_PI 0.318309886

float tau_integral(float x) {
	return (x*sqrt(-x * x + 1) + asin(x))*ONE_OVER_PI + 0.5;
}

RayCone CreateConeOnSpheres(vec4 s0, vec4 s1)
{
    vec3 v = s1.xyz - s0.xyz;
    float d = length(v);
    vec2 R = vec2(s0.w, s1.w)*d / sqrt(d*d - (s1.w-s0.w) * (s1.w-s0.w));

    RayCone raycone;
    raycone.ray.Origin      = s0.xyz;
    raycone.ray.Direction   = v/d;
    raycone.ray.Tmin        = s0.w;
    raycone.ray.Tmax        = d-s1.w;
    raycone.rad				= R.x;
    raycone.tana			= (R.y-R.x)/d;
    return raycone;
}

TraceResult DefaultMinConeTrace(inout RayCone raycone, SphereTraceDesc params)
{
    TraceResult ret = TraceResult( raycone.ray.Tmin, 0 ); //t_min
    float t = raycone.ray.Tmin;
    float m = 1; // 1 --> -1 we are looking for the minimum in terms of 'm'...
    vec4 data = vec4(0);
    float R = raycone.rad, A = raycone.tana; //save as 'raycone' changes
    //raycone.rad = (m - params.epsilon) * R;
    //raycone.tana = (m - params.epsilon) * A;
    int i = 0; do
    {
        DefaultConeStep(raycone, t, data);
        //SmartConeStep(raycone, t, data);
        if (extract_dist(data) < params.epsilon * A * t)
        {
            m = (extract_dist(data) + raycone.rad + raycone.tana * t) / (R+A*t);
            raycone.rad  = (m - params.epsilon) * R;
            raycone.tana = (m - params.epsilon) * A;
            ret.T = t;
        }
        ++i;
    } while (
		    t < raycone.ray.Tmax    && // Stop if the light is reached
		    m > params.epsilon      && // When m < 0, convex optimization dies
		    i < params.maxiters        // Stop if too many iterations
		);
	
    while (
		    t < raycone.ray.Tmax    && // Stop if the light is reached
		    m > -1                  && // Stop when we reach the surface for real
		    i < params.maxiters        // Stop if too many iterations
		)
    {
        NegativeConeStep(raycone, t, data);
        //SmartConeStep(raycone, t, data);
        if (extract_dist(data) < params.epsilon * A * t)
        {
            m = (extract_dist(data) + raycone.rad + raycone.tana * t) / (R + A * t);
            raycone.rad = (m - params.epsilon) * R;
            raycone.tana = (m - params.epsilon) * A;
            ret.T = t;
        }
        ++i;
    }

    ret.flags = int(t >= raycone.ray.Tmax)         //light reached
              | (int(m <= -1) << 1)                //full shadow
              | (int(i >= params.maxiters) << 2);  //out of iterations
    return ret;
}


float shadow_soft_new(vec4 sphere0, vec4 sphere1, out bool outOfIters)
{
    RayCone raycone = CreateConeOnSpheres(sphere0, sphere1);
    SphereTraceDesc params;
        params.maxiters = gShadowMaxiter;
        params.epsilon = gShadowEpsilon;
    float R = raycone.rad;
    TraceResult tf = DefaultMinConeTrace(raycone, params);
    outOfIters = bool(tf.flags & 4);
	float m = clamp(raycone.rad/R, -1, 1);
    return tau_integral(m);
}

