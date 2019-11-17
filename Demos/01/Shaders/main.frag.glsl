#version 130

in vec4 vs_out_col;
out vec4 fs_out_col;

uniform vec3 gCameraPos;
subroutine vec3 calcColor_t();
subroutine uniform calcColor_t calcColor;

uniform sampler3D testField;

void main()
{
    fs_out_col = vec4(calcColor(), 1)*.5;
	fs_out_col += texture(testField, vec3(vs_out_col.xy, gCameraPos.z))*.5;
}

subroutine (calcColor_t)
vec3 textureColor()
{
	return texture(testTexture, vs_out_col.xy).xyz;
}

subroutine (calcColor_t)
vec3 vertColor()
{
	return vs_out_col.xyz;
}












