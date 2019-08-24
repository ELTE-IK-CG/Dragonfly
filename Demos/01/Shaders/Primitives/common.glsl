#define SQRT2 1.4142135623
#define PI 3.14159265359
#define PI2 (2*3.14159265359)

#pragma warning(disable : 4008)

// !!! Breaks the convex optimizer: !!!!
// #define repeat(p, r)   ( fmod(p + r * 0.5, r) - r * 0.5 )
// #define repeatXY(p, r) vec3( repeat( (p).xy, r), (p).z )
// #define repeatYZ(p, r) vec3( repeat( (p).yz, r), (p).x )
// #define repeatXZ(p, r) vec3( repeat( (p).xz, r), (p).y )

#define INF 3.402823466e+38    //whats the maximum?
#define CONVEXOPT_FPOS(f, vdf) vec2(f, (vdf < 0.0 ? -f/vdf : INF) )
#define CONVEXOPT(f, vdf) vec2(f, f < 0.0 ? f : (vdf < 0.0 ? -f/vdf : INF) )

// Planes
float plane(vec3 p, vec3 n)
{
    return dot(p, normalize(n));
}
vec2 plane(vec3 p, vec3 v, vec3 n)
{
    float f = dot(p, n);
    float vdf = dot(v, normalize(n));
    return CONVEXOPT(f, vdf);
}
// Axis aligned planes
float planeYZ(vec3 p){return p.x;}
vec2 planeYZ(vec3 p, vec3 v){return CONVEXOPT(p.x, v.x);}
float planeXZ(vec3 p){return p.y;}
vec2 planeXZ(vec3 p, vec3 v){return CONVEXOPT(p.y, v.y);}
float planeXY(vec3 p){ return p.y;}
vec2 planeXY(vec3 p, vec3 v){return CONVEXOPT(p.z, v.z);}

// Sphere
float sphere(vec3 p, float r)
{
    return length(p) - r;
}
vec2 sphere(vec3 p, vec3 v, float r)
{
    float f = length(p) - r;
    float vdf = dot(v, normalize(p));
    return CONVEXOPT(f, vdf);
}

// Infinite cylinders
float cylinderZ(vec3 p, float r)
{
    return length(p.xy) - r;
}
vec2 cylinderZ(vec3 p, vec3 v, float r)
{
    float f = length(p.xy) - r;
    float vdf = dot(v, normalize(vec3(p.xy,0)));
    return CONVEXOPT(f, vdf);
}
float  cylinderX(vec3 p, float r){return cylinderZ(p.zyx,r);}
vec2 cylinderX(vec3 p, vec3 v, float r){return cylinderZ(p.zyx, v.zyx, r);}
float  cylinderY(vec3 p, float r){return cylinderZ(p.xzy,r);}
vec2 cylinderY(vec3 p, vec3 v, float r){return cylinderZ(p.xzy, v.xzy, r);}

// Finite cylinders
float cylinderZ(vec3 p, vec2 h)
{
    vec2 d = abs(vec2(length(p.xy), abs(p.z))) - h;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}
vec2 cylinderZ(vec3 p, vec3 v, vec2 h)
{
    float pxylen = length(p.xy);
    vec2 d = vec2(pxylen, abs(p.z)) - h;
    vec2 ed = max(d, 0.0);
    float edlen = length(ed);
    vec3 dd = vec3(ed.x * p.xy, (p.z > 0.0 ? ed.y : -ed.y) * pxylen);
    float f = min(max(d.x, d.y), 0.0) + edlen;
    float vdf = dot(v, dd) / (edlen * pxylen);
    return CONVEXOPT(f, vdf);
}
float  cylinderX(vec3 p, vec2 h){return cylinderZ(p.zyx,h);}
vec2 cylinderX(vec3 p, vec3 v, vec2 h){return cylinderZ(p.zyx, v.zyx, h);}
float  cylinderY(vec3 p, vec2 h){return cylinderZ(p.xzy,h);}
vec2 cylinderY(vec3 p, vec3 v, vec2 h){return cylinderZ(p.xzy, v.xzy, h);}

// Box
float box(vec3 p, vec3 size)
{
    vec3 d = abs(p) - size;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}
vec2 box(vec3 p, vec3 v, vec3 size)
{
    vec3 d = abs(p) - size; //signed distances to planes
    //vec3 dd = sign(p) * max(d, 0); //outside 'normal' // sign is slow...
    //vec3 dd = (p < 0 ? vec3(-1) : vec3(1)) * max(d, 0); //outside 'normal'
    vec3 dd = (2.0 * vec3(greaterThan(p, vec3(0.0))) - 1.0) * max(d, 0.0); //outside 'normal'
    float lendd = length(dd);
    float f = lendd + min(max(d.x, max(d.y, d.z)), 0.0); //one is always zero
    float vdf = dot(v, dd) / lendd; //<f',v>
    return CONVEXOPT(f, vdf);
}
// might be faster if simplifyed: 
//vec2 box(vec3 p, vec3 v, vec3 size)
//{
//    vec3 p0 = min(size, abs(p));
//    vec3 dd = p - p0 * (2 * vec3(p > 0) - 1);
//    vec3 d = abs(p) - size;
//    float lendd = length(dd);
//    float f = min(max(d.x, max(d.y, d.z)), 0.0) + lendd;
//    float vdf = dot(v, dd) / lendd;
//    return CONVEXOPT(f, vdf);
//}

// ************************
//      SET OPERATIONS
// ________________________

float Offset(float f, float r){return f - r;}

//Negative offsets only work on convex objects, and even then they are sub-obtimal !!
vec2 Offset(vec2 f_cf, float r)
{
    float f = f_cf.x - r;
    return vec2(f, f < 0.0 ? f : f_cf.y * f / f_cf.x);
}

float Union(float d1, float d2){ return min(d1,d2);}
//vec2 Union(vec2 di1, vec2 di2){ return di1.x < di2.x ? di1 : di2;}
vec2 Union(vec2 di1, vec2 di2){ return min(di1,di2);}

float Intersect(float d1, float d2){ return max(d1,d2);}
vec2 Intersect(vec2 di1, vec2 di2){ return di1.x > di2.x ? di1 : di2;}
//vec2 Intersect(vec2 di1, vec2 di2){ return max(di1,di2);}

float Substract(float d1, float d2){return max(d1,-d2);}
vec2 Substract(vec2 di1, vec2 di2){ return di1.x > -di2.x ? di1 : -di2;}
//vec2 Substract(vec2 di1, vec2 di2){ return max(di1,-di2);}