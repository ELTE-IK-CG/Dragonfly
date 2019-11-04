#version 130

in vec4 vs_out_col;
out vec4 fs_out_col;

subroutine vec3 calcColor_t();
subroutine uniform calcColor_t calcColor;

void main()
{
	fs_out_col = vec4(calcColor(), 1);
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












