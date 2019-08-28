uniform float col_intensity;
uniform vec3 gCameraPos;

const vec2 positions[3] = vec2[3](
	vec2(-1,-1),
	vec2( 1,-1),
	vec2(0, 1)
);
in vec2 vs_in_pos;
out vec4 vs_out_col;

void main()
{
	//gl_Position  = vec4(positions[gl_VertexID],0,1);
	gl_Position = vec4(vs_in_pos,0, 1);
	vs_out_col = vec4(col_intensity*gl_VertexID);
	vs_out_col.xy = gCameraPos.xy*col_intensity;
}