// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform vec3 u_lightPosition;

out vec3 vecN;
out vec3 vecL;

void main()
{
	vec3 v_eye = vec3(u_mv * a_position);

	// Calculate the view-space normal
	vecN = normalize(mat3(u_mv) * a_normal);

	// Calculate the view-space light direction
	vec3 l_vecLight = vec3(mat3(u_mv) * u_lightPosition) ;
	vecL = normalize(normalize(l_vecLight) - v_eye);
	
	gl_Position = u_mvp * a_position;

}
