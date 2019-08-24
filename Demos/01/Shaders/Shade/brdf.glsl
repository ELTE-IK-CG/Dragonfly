//?#version 450
//?#include "../types.hlsl"

struct Material
{
    vec3 ambient;
    vec3 diffcol;
    vec3 roughness;
    vec3 ior;
};

vec3 cook_torrance(vec3 V, vec3 N, vec3 L, Material mat)
{
    float NV = max(0, dot(N, V));
    vec3 H = normalize(V + L);
    float NH = max(0, dot(N, H)); // we are dividing by N dot H; if it's <= 0, we should return with nothing
    float NL = max(0, dot(N, L));
    if (NV <= 0 || NH <= 0 || NV <= 0 || NL <= 0 )
        return vec3(0, 0, 0);

    float VH = max(0, dot(V, H));

	// Fresnel reflectance
    vec3 F0 = (1 - mat.ior) / (1 + mat.ior);
    F0 *= F0;
    vec3 F = F0 + (1 - F0) * pow(1 - VH, 5.0);

    // Microfacet distribution by Beckmann
    vec3 m2 = mat.roughness * mat.roughness;
	float NH2 = NH * NH;
    float tana2 = (1 - NH2) / NH2;
    vec3 D = exp(-tana2 / m2) / (PI * m2 * NH2 * NH2);

	// Geometric shadowing
    float G = min(1, 2 * NH * min(NV, NL) / VH);
    
    vec3 spec = max((F * D * G) / (NV * NL), 0); //geometric term is devided by 4 already

    vec3 diff = vec3(clamp(NL, 0.0, 1.0));

    return (spec+diff)*mat.diffcol; //ambient cannot be included here
}
