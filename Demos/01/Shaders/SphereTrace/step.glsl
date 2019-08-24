//?#version 450
//! #include "../types.glsl"
//! #include "../Primitives/common.glsl"
//! #include "../sdf.glsl"

float detail_eh_stepsize(vec3 r0dr1) //omega = 0.9
{
    return 0.9 * r0dr1.z * max((r0dr1.y - r0dr1.x + r0dr1.z) / (r0dr1.y + r0dr1.x - r0dr1.z), 0.0);
}
vec2 detail_cx_ct_step(RayCone raycone, float t)
{
    vec2 ff = sdf(RAY(raycone.ray, t), raycone.ray.Direction);
    float off = (ff.x - t * raycone.tana - raycone.rad);
    return off / vec2(1 + abs(raycone.tana), ff.x / ff.y + raycone.tana);
}

#define extract_prev_dist2surf(data) data.x
#define extract_curr_dist2surf(data) data.z //This is the one usually needed
#define extract_prev_stepsize(data) data.y
#define extract_next_stepsize(data) data.w

#define extract_dist(data) extract_curr_dist2surf(data)

const float gOmegaC = 3.0;

// Zero-overhead one-step enhanced conetrace. Init data to 0. See next function for usage. Macros for data above.
// Cone must not be inverted/negative!
void DefaultConeStep(RayCone raycone, inout float t, inout vec4 data)
{
        //Enhanced step would take the leap:
    float enhstep = data.z + detail_eh_stepsize(data.xyz);
    
        //Enhanced may not surely work, so only use it if we have much to gain
    float tostep = (enhstep > gOmegaC * data.w ? enhstep : data.w);

        //Evaluate sdf at the furthest point max(enhanced, convex)
    vec2 rd2 = detail_cx_ct_step(raycone, t + tostep);

        // We may have taken an enhanced step. If we did, we have to check if its valid.
    if (enhstep > gOmegaC * data.w && data.z + rd2.x < enhstep)
    {
        //Enhanced step was invalid, step back
        rd2 = detail_cx_ct_step(raycone, t + data.w);
    }
    else
        data.w = tostep;

        //Increasing t and rotating variables
    t += data.w;    data.xy = data.zw;    data.zw = rd2;
}

// Best used for negative cones
void NegativeConeStep(RayCone raycone, inout float t, inout vec4 data)
{
    vec3 temp; // vec3(cone 2 surface distance, next step size, temporary sdf value) (sorry)
    temp.y = data.z + detail_eh_stepsize(data.xyz);
    temp.z = SDF(raycone.ray, t + temp.y);
    temp.x = CONE_SDF_FOS(temp.z, raycone, t+temp.y);
    if (data.z + temp.x < temp.y)
    {
        temp.z = SDF(raycone.ray, t+data.z);
        temp.xy = vec2(CONE_SDF_FOS(temp.z, raycone, t+data.z), data.z);
    }
    t += temp.y;
    data.xy = data.zw;
    data.zw = temp.xy;
}

// Uses both -- Generally inefficient -- Use it for negative cones and later optimize -- Ask Robi or Csaba how to.
void SmartConeStep(RayCone raycone, inout float t, inout vec4 data)
{
	if(raycone.tana*t  + raycone.rad < 0.0)
        NegativeConeStep(raycone, t, data);
	else
        DefaultConeStep(raycone, t, data);
}

void DefaultRayStep(RayDesc ray, inout float t, vec4 data)
{
    RayCone raycone = RayCone(ray, 0.0, 0.0);
    DefaultConeStep(raycone, t, data);

    /*  Doest work for some reason: TODO DEBUG

        //Enhanced step would take the leap:
    float enhstep = data.z + detail_eh_stepsize(data.xyz);
    
        //Enhanced may not surely work, so only use it if we have much to gain
    float tostep = (enhstep > gOmegaC * data.w ? enhstep : data.w);

        //Evaluate sdf at the furthest point max(enhanced, convex)
    vec2 rd2 = sdf(ray.Origin + (t + tostep) * ray.Direction, ray.Direction);

        // We may have taken an enhanced step. If we did, we have to check if its valid.
    if (enhstep > gOmegaC * data.w && data.z + rd2.x < enhstep)
    {
        //Enhanced step was invalid, step back
        rd2 = sdf(ray.Origin + (t + data.w) * ray.Direction, ray.Direction);
    }
    else
        data.w = tostep;

    //Increasing t and rotate variables
    t += data.w;    data.xy = data.zw;  data.zw = rd2;
    */
}

