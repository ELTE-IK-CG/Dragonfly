#version 130

in vec4 vs_out_col;
out vec4 fs_out_col;

uniform vec3 gCameraPos;

uniform sampler3D testField;

void main()
{
    fs_out_col = 0.8*texture(testTexture, vs_out_col.xy);
	fs_out_col += 0.2*texture(testField, vec3(vs_out_col.xy, gCameraPos.z));
    fs_out_col += vs_out_col;
}















