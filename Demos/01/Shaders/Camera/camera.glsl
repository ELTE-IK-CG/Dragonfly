//?#version 450
//?#include "..\types.glsl"

// vec4(dir, tana)

RayCone SimpleCamera(float2 pix)
{
	float3 w = normalize(gCameraDir);
	float3 u = normalize(cross(gCameraUp, w));
	float3 v = cross(w, u);
	float lenres = length(gResolution);
	float2 pix2 = pix * gTanFov * gResolution / lenres;

    RayCone cone;
    cone.ray.Tmin = 0.0;
    cone.ray.Origin = gCameraPos;
    cone.ray.Tmax = 10000.0;
	cone.ray.Direction = normalize(w + pix2.x*u+pix2.y*v);
	cone.tana = sqrt(2)*gTanFov / lenres;
    cone.rad = 0.0;
    return cone;
}