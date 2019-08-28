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

void sdf(vec3 p, vec3 v)
{

}
//not convex optimized version
//float sdf(vec3 p){return sdf(p,vec3(0,0,0)).x;}


