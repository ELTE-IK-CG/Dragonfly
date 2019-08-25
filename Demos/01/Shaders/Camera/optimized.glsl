//?#version 450
//?#include "..\types.glsl"

uniform mat4 gInverseViewProj;
uniform vec3 gCameraPos;

RayDesc OptimizedCamera(vec2 pix)	//todo move everything to vertex shader besides normalizing the direction
{
	vec4 near = gInverseViewProj * vec4(pix, -1, 1);
	vec4 fear = gInverseViewProj * vec4(pix, +1, 1); //far
	vec3 rayOri = near.xyz/near.w;
	vec3 rayEnd = fear.xyz/fear.w;
	RayDesc ray;
	ray.Origin = gCameraPos; //gCameraPos, rayOri, and rayEnd should be colinear
	ray.Direction = normalize(rayEnd - rayOri);
	ray.Tmin = distance(gCameraPos, rayOri);
	ray.Tmax = distance(gCameraPos, rayEnd);
	return ray;
}
