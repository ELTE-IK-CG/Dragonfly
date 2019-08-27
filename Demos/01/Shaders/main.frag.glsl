//!#version 450

//!#include "types.glsl"
//!#include "Primitives/common.glsl"
//!#include "sdf.glsl"
//!#include "SphereTrace/step.glsl"
//!#include "SphereTrace/trace.glsl"
//!#include "AmbientOcclusion/basic.glsl"
//!#include "Shadow/softshadow.glsl"
//!#include "Shade/brdf.glsl"
//!#include "Shade/basic.glsl"

in vec4 vs_out_col;
out vec4 fs_out_col;

void main()
{
    fs_out_col = vs_out_col;

}









