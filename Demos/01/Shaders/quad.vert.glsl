#version 330
uniform float col_intensity;
uniform vec3 gCameraPos;

layout(location = 0) in vec3 vs_in_pos;
out vec4 vs_out_col;

void main()
{
	gl_Position  = vec4(vs_in_pos, 1);
	vs_out_col = vec4(vs_in_pos,1);
	vs_out_col.xy += gCameraPos.xy*col_intensity;
}
























