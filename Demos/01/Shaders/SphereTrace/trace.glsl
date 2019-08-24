//?#version 450
//!#include "../types.glsl"
//!#include "../Primitives/common.glsl"
//!#include "../sdf.glsl"
//!#include "step.glsl"

TraceResult DefaultConeTrace(RayCone cone, SphereTraceDesc params)
{
    TraceResult ret = TraceResult(cone.ray.Tmin, 0);
    vec4 data = vec4(0.0); // for embedded step algorithm
    
    int i = 0; do
    {
        DefaultConeStep(cone, ret.T, data);
        ++i;
    } while (
		ret.T               < cone.ray.Tmax &&       // Stay within bound box
		extract_dist(data)  > params.epsilon * ret.T && // Stop if cone is close to surface
		i                   < params.maxiters	        // Stop if too many iterations
	);
    ret.flags = int(ret.T >= cone.ray.Tmax)
              | (int(extract_dist(data) <= params.epsilon) << 1)
              | (int(i >= params.maxiters) << 2);
    return ret;
}

TraceResult NegativeConeTrace(RayCone cone, SphereTraceDesc params)
{
    TraceResult ret = TraceResult(cone.ray.Tmin, 0);
    vec4 data = vec4(0); // for embedded step algorithm
    
    int i = 0; do
    {
        NegativeConeStep(cone, ret.T, data);
        ++i;
    } while (
		ret.T               < cone.ray.Tmax &&       // Stay within bound box
		extract_dist(data)  > params.epsilon * ret.T && // Stop if cone is close to surface
		i                   < params.maxiters	        // Stop if too many iterations
	);
    ret.flags = int(ret.T >= cone.ray.Tmax)
              | (int(extract_dist(data) <= params.epsilon) << 1)
              | (int(i >= params.maxiters) << 2);
    return ret;
}

//Smart is the new slow... -- could be optimized, but would still be slow
TraceResult SmartConeTrace(RayCone cone, SphereTraceDesc params)
{
    TraceResult ret = TraceResult(cone.ray.Tmin, 0);
    vec4 data = vec4(0); // for embedded step algorithm
    
    int i = 0;    do
    {
        SmartConeStep(cone, ret.T, data);
        ++i;
    } while (
		ret.T               < cone.ray.Tmax &&       // Stay within bound box
		extract_dist(data)  > params.epsilon * ret.T && // Stop if cone is close to surface
		i                   < params.maxiters	        // Stop if too many iterations
	);
    ret.flags = int(ret.T               >= cone.ray.Tmax)
              | (int(extract_dist(data) <= params.epsilon)  << 1)
              | (int(i                  >= params.maxiters) << 2);
    return ret;
}

TraceResult DefaultRayTrace(RayDesc ray, SphereTraceDesc params)
{
    RayCone cone = RayCone(ray, 0, 0); // BUGS ALL OVER
    return DefaultConeTrace(cone, params);

    /* Doesnt work for some reason: TODO DEBUG

    TraceResult ret = TraceResult(cone.ray.Tmin, 0);
    vec4 data = vec4(0); // for embedded step algorithm
    
    int i = 0;  do
    {
        DefaultRayStep(ray, ret.T, data);
        ++i;
    } while (
		ret.T               < ray.Tmax &&               // Stay within bound box
		extract_dist(data)  > params.epsilon * ret.T && // Stop if cone is close to surface
		i                   < params.maxiters	        // Stop if too many iterations
	);
    ret.flags = (ret.T               >= ray.Tmax)
              | ((extract_dist(data) <= params.epsilon)  << 1)
              | ((i                  >= params.maxiters) << 2);
    return ret;
    */
}
