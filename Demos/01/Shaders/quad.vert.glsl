//!#version 450

// quad version:
const vec2 positions[3] = vec2[3](
	vec2(-1,-1),
	vec2( 1,-1),
	vec2(0, 1)
);

out vec4 vs_out_col;

void main()
{
	gl_Position  = vec4(positions[gl_VertexID],0,1);
	vs_out_col = vec4(0.5*gl_VertexID);
}