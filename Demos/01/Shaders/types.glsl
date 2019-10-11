#version 450
#define RAY(r,t) (r.Direction*(t)+r.Origin)
#define SDF(r,t) sdf(RAY(r,t))
#define CONE_SDF(raycone, t) ((SDF(raycone.ray,t)-(t)*raycone.tana-raycone.rad)/(1.0+raycone.tana)) 
#define CONE_SDF_FOS(fos, raycone, t) (((fos)-(t)*raycone.tana-raycone.rad)/(1.0+raycone.tana)) 

uniform sampler2D testTexture;

struct RayDesc
{
	vec3 Origin;
	float Tmin;
	vec3 Direction;
	float Tmax;
};

struct RayCone
{
    RayDesc ray;    //Central axis of the cone
	float tana;		//Cone opening angle when cone tracing can be zero
	float rad;		//Cone starting radius at Origin (t=0)
};

struct TraceResult
{
    float T;		// Distance taken on ray
    int flags;		// bit 0:   distance condition:     true if travelled to far t > t_max
                    // bit 1:   surface condition:      true if distance to surface is small < error threshold
                    // bit 2:   iteration condition:    true if took too many iterations
//    int iterations;	// Iterations taken
//    float distance;   // Distance to the surface (lower bound)
};

struct SphereTraceDesc
{
    float epsilon;  //Cone stopping distance to surface
    int maxiters;   //Maximum iteration count
};








