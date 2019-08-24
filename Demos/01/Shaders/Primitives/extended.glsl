//!#include "common.glsl"

// Infinite approximate cones
//      Faster for larger cones. Variable c must be normalized. Same as normalize(1, tana).
float fastConeZ(vec3 p, vec2 c)
{
    float q = length(p.xy);
    return dot(normalize(c), vec2(q, p.z));
}
vec2 fastConeZ(vec3 p, vec3 v, vec2 c)
{
    float q = length(p.xy);
    float f = dot(c, vec2(q, p.z));
    vec3 n = vec3(p.xy / q * c.x, c.y);
    float vdf = dot(v, n);
    return CONVEXOPT(f, vdf);
}
float  fastConeX(vec3 p, vec2 c){return fastConeZ(p.zyx, c);}
vec2 fastConeX(vec3 p, vec3 v, vec2 c){return fastConeZ(p.zyx, v.zyx, c);}
float  fastConeY(vec3 p, vec2 c){return fastConeZ(p.xzy, c);}
vec2 fastConeY(vec3 p, vec3 v, vec2 c){return fastConeZ(p.xzy, v.xzy, c);}

// Finite cones
//      c=(radius, half height)
float coneZ(vec3 p, vec2 c)
{
    vec2 p2 = vec2(length(p.xy), p.z);
    vec2 n0 = normalize(vec2(2.0 * c.y, c.x));
    vec2 c2p = p2 - (p2.y > 0.0 ? vec2(0.0, +c.y) : vec2(c.x, -c.y));
    vec2 nn = normalize(vec2(max(c2p.x, 0.0), c2p.y));
    if (dot(nn, vec2(n0.y, -n0.x)) * p2.y > 0.0) {nn = n0;}
    return dot(nn, c2p);
}
vec2 coneZ(vec3 p, vec3 v, vec2 c)
{
    vec2 p2 = vec2(length(p.xy), p.z);
    vec2 n0 = normalize(vec2(2.0 * c.y, c.x));
    vec2 c2p = p2 - (p2.y > 0.0 ? vec2(0, +c.y) : vec2(c.x, -c.y));
    vec2 nn = normalize(vec2(max(c2p.x, 0.0), c2p.y));
    if (dot(nn, vec2(n0.y, -n0.x)) * p2.y > 0.0) { nn = n0; }
    float f = dot(nn, c2p);
    vec3 n3 = vec3(nn.x * p.xy / p2.x, nn.y);
    f = f < 0.0 ? max(dot(n0, c2p), -p2.y - c.y) : f;
    float vdf = dot(n3, v);
    return CONVEXOPT(f, vdf);
}
float  coneX(vec3 p, vec2 c){return coneZ(p.zyx, c);}
vec2 coneX(vec3 p, vec3 v, vec2 c){return coneZ(p.zyx, v.zyx, c);}
float  coneY(vec3 p, vec2 c){return coneZ(p.xzy, c);}
vec2 coneY(vec3 p, vec3 v, vec2 c){return coneZ(p.xzy, v.xzy, c);}

// Finite capped cones
//      c = (radius 1, half height, radius 2)
float cappedConeZ(vec3 p, vec3 c)
{
    vec2 p2 = vec2(length(p.xy), p.z);
    vec2 n0 = normalize(vec2(2.0 * c.y, c.z - c.x));
    vec2 c2p = p2 - (p2.y > 0.0 ? vec2(c.x, +c.y) : vec2(c.z, -c.y));
    vec2 nn = normalize(vec2(max(c2p.x, 0.0), c2p.y));
    if (dot(nn, vec2(n0.y, -n0.x)) * p2.y > 0.0) { nn = n0; }
    float f = dot(nn, c2p);
    return f < 0.0 ? max(dot(n0, c2p), abs(p2.y) - c.y) : f;
}
vec2 cappedConeZ(vec3 p, vec3 v, vec3 c)
{
    vec2 p2 = vec2(length(p.xy), p.z);
    vec2 n0 = normalize(vec2(2.0 * c.y, c.z - c.x));
    vec2 c2p = p2 - (p2.y > 0.0 ? vec2(c.x, +c.y) : vec2(c.z, -c.y));
    vec2 nn = normalize(vec2(max(c2p.x, 0.0), c2p.y));
    if (dot(nn, vec2(n0.y, -n0.x)) * p2.y > 0.0) { nn = n0; }
    float f = dot(nn, c2p);
    vec3 n3 = vec3(nn.x * p.xy / p2.x, nn.y);
    f = f < 0.0 ? max(dot(n0, c2p), abs(p2.y) - c.y) : f;
    float vdf = dot(n3, v);
    return CONVEXOPT(f, vdf);
}
float  cappedConeX(vec3 p, vec3 c){return cappedConeZ(p.zyx, c);}
vec2 cappedConeX(vec3 p, vec3 v, vec3 c){return cappedConeZ(p.zyx, v.zyx, c);}
float  cappedConeY(vec3 p, vec3 c){return cappedConeZ(p.xzy, c);}
vec2 cappedConeY(vec3 p, vec3 v, vec3 c){return cappedConeZ(p.xzy, v.xzy, c);}

// Fast finite cones (not exact!)

float fastCappedConeZ(vec3 p, vec3 c)
{
    vec2 p2 = vec2(length(p.xy), p.z);
    vec2 n0 = normalize(vec2(2.0 * c.y, c.z - c.x));
    vec2 c2p = p2 - (p2.y > 0.0 ? vec2(c.x, +c.y) : vec2(c.z, -c.y));
    return max(dot(n0, c2p), abs(p2.y) - c.y);
}
vec2 fastCappedConeZ(vec3 p, vec3 v, vec3 c)
{
    vec2 p2 = vec2(length(p.xy), p.z);
    vec2 n0 = normalize(vec2(2.0 * c.y, c.z - c.x));
    vec2 c2p = p2 - (p2.y > 0.0 ? vec2(c.x, +c.y) : vec2(c.z, -c.y));
    float f = max(dot(n0, c2p), abs(p2.y) - c.y);
    vec3 n3 = vec3(n0.x * p.xy / p2.x, n0.y);
    float vdf = (f == dot(n0, c2p) ? dot(n3, v) : (p2.y > 0.0 ? v.z : -v.z));
    return CONVEXOPT(f, vdf);
}
float  fastCappedConeX(vec3 p, vec3 c){return fastCappedConeZ(p.zyx, c);}
vec2 fastCappedConeX(vec3 p, vec3 v, vec3 c){return fastCappedConeZ(p.zyx, v.zyx, c);}
float  fastCappedConeY(vec3 p, vec3 c){return fastCappedConeZ(p.xzy, c);}
vec2 fastCappedConeY(vec3 p, vec3 v, vec3 c){return fastCappedConeZ(p.xzy, v.xzy, c);}

// Work in progress:

//float pyramid(vec3 p, vec2 c){}

//Reconstructs a vector v given its two projections, so v is such that <v,a>=<a,a> and <v,b>=<b,b>.
vec3 deproject(vec3 a, vec3 b)
{
    float ab = dot(a,b), aa = dot(a,a), bb = dot(b,b);
    return (bb*(aa-ab)*a + aa*(bb-ab)*b) / (aa*bb - ab*ab);
}
// a and b is unit length, c.x and c.y is the length of the projections
vec3 deproject(vec3 a, vec3 b, vec2 c)
{
    float ab = dot(a, b);
    return ((c.x - c.y*ab) * a + (c.y - c.x*ab) * b) / (1.0 - ab*ab);
}

vec2 edgeConvex(vec3 p, vec3 v, vec3 n1, vec3 n2)
{
    vec3 m = normalize(cross(n1, n2));
    vec3 pp = p - dot(p, m) * m;
    float lenpp = length(pp);
    float p1 = dot(p, n1), p2 = dot(p, n2), cn = dot(n1, n2);
            
    vec2 fv = max(p1, p2)  < 0.0 ? vec2(1, 1) * max(p1, p2) :
                p2 - cn * p1 < 0.0 & p1 > 0.0 ? vec2(p1, dot(v, n1)) :
                p1 - cn * p2 < 0.0 & p2 > 0.0 ? vec2(p2, dot(v, n2)) :
                                   vec2(lenpp, dot(v, pp) / lenpp);
    return CONVEXOPT(fv.x, fv.y);
}

// dot(n1,n2) >= 0
vec2 edgeConvexBlunt(vec3 p, vec3 v, vec3 n1, vec3 n2)
{
    vec3 m = normalize(cross(n1, n2));
    vec3 pp = p - dot(p, m) * m;
    float lenpp = length(pp);
    float p1 = dot(p, n1), p2 = dot(p, n2), cn = dot(n1, n2);
    vec2 fv = max(p1, p2)  < 0.0 ? vec2(1,1) * max(p1, p2) :
                p2 - cn * p1 < 0.0 ? vec2(p1, dot(v, n1)) :
                p1 - cn * p2 < 0.0 ? vec2(p2, dot(v, n2)) :
                                   vec2(lenpp, dot(v, pp)/lenpp);
    return CONVEXOPT(fv.x, fv.y);
}

// Best for internal use
vec2 edgeConvexBluntOutside(vec3 p, vec3 v, vec3 n1, vec3 n2)
{
    vec3 m = normalize(cross(n1, n2));
    vec3 pp = p - dot(p, m) * m;
    float lenpp = length(pp);
    float p1 = dot(p, n1), p2 = dot(p, n2), cn = dot(n1, n2);
    vec2 fv = p2 - cn * p1 < 0.0 ? vec2(p1, dot(v, n1)) :
                p1 - cn * p2 < 0.0 ? vec2(p2, dot(v, n2)) :
                                   vec2(lenpp, dot(v, pp) / lenpp);
    return CONVEXOPT_FPOS(fv.x, fv.y);
}

//c=(width along x, width along y, height along z)
vec2 pyramidZ(vec3 p, vec3 v, vec3 c)
{
    v.xy *= sign(p.xy);    p.xy = abs(p.xy); //solve only one quarter
    vec3 nx = normalize(vec3(c.z, 0, c.x)), ny = normalize(vec3(0, c.z, c.y));   // normals of the sides
    vec2 mxy = c.z*vec2(nx.z,ny.z);           // distance to sides from origo
    vec2 dxy = vec2(dot(nx, p), dot(ny, p)) - mxy;

    float din = max(max(dxy.x, dxy.y), -p.z);
    if (din < 0.0){ return vec2(din,din);}

    vec3 t1 = vec3(p.xy, p.z - c.z);
    vec2 fv = edgeConvexBluntOutside(t1, v, nx, ny);

    fv = dot(t1, vec3(-c.xy, c.z)) < 0.0 ? fv :
            CONVEXOPT_FPOS(length(t1), dot(v, normalize(t1)));

    vec3 t2 = p - vec3(min(p.xy, c.xy), 0);
    fv = dot(t2, vec3(c.x, 0, -c.z)) < 0.0 && dot(t2, vec3(0.0, c.y, -c.z)) < 0.0 ? fv :
            CONVEXOPT_FPOS(length(t2), dot(v, normalize(t2)));
    return fv;
}
vec2 pyramidX(vec3 p, vec3 v, vec3 c){return pyramidZ(p.zyx, v.zyx, c);}
vec2 pyramidY(vec3 p, vec3 v, vec3 c){return pyramidZ(p.xzy, v.xzy, c);}


//1D

vec2 line(vec3 p, vec3 v, vec3 dir)
{
    p -= dot(p, dir) * dir;
    float l = length(p);
    vec2 fv = vec2(l, dot(v, p)/l);
    return CONVEXOPT_FPOS(fv.x, fv.y);
}

vec2 ray(vec3 p, vec3 v, vec3 dir)
{
    p -= max(dot(p, dir),0.0) * dir;
    float l = length(p);
    vec2 fv = vec2(l, dot(v, p) / l);
    return CONVEXOPT_FPOS(fv.x, fv.y);
}

vec2 segment(vec3 p, vec3 v, vec3 a, vec3 b)
{
    p -= a;    b -= a;
    float l2 = dot(b,b);
    p -= clamp(dot(b, p) / l2,0,1) * b;
    float l = length(p);
    vec2 fv = vec2(l, dot(v, p) / l);
    return CONVEXOPT_FPOS(fv.x, fv.y);
}

// Not yet convex optimized:

// faster for
float torus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xy) - t.x, p.z);
    return length(q) - t.y;
}

//Other primitives:

vec2 cyliner(vec3 p, vec3 v, vec3 dir, float r)
{
    return Offset(line(p, v, dir), r);
}

vec2 capsule(vec3 p, vec3 v, vec3 a, vec3 b, float r)
{
    return Offset(segment(p, v, a, b), r);
}
