//?#version 450
//!#include "Primitives/common.glsl"

//convex optimized
//must be exactly the same implementation, but with float2 and passing v around
//if you rotate p, don't forget to rotate v too
//float2 sdf(float3 p, float3 v);

//#include "SDF/primitives_demo.hlsl"
//#include "SDF/mandelbulb.hlsl"
//#include "SDF/simple_test.hlsl"
//#include "SDF/spheres.hlsl"

vec2 sdf(vec3 p, vec3 v)
{
    //SDF for weak GPUs
    vec2 d = planeXZ(p, v);
    p.y -= 1.0;
    d = Union(d, box(p, v, vec3(1, 1, 1)));
    p.x -= 2.0;
    d = Union(d, sphere(p, v, 1.0));
    p.x += 4.0;
    d = Union(d, cylinderY(p, v, vec2(1, 1)));
    return d;
}
//not convex optimized version
float sdf(vec3 p){return sdf(p,vec3(0,0,0)).x;}

