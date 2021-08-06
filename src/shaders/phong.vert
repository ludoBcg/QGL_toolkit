// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_color;
layout(location = 3) in vec2 a_uv;
layout(location = 4) in vec3 a_tangent;
layout(location = 5) in vec3 a_bitangent;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform vec3 u_lightPosition;

out vec3 vecN;
out vec3 vecL;
out vec3 vecV;
out vec3 vecT;
out vec3 vecBT;
out vec3 vert_pos;
out vec3 vert_uv;


void main()
{
	vec3 v_eye = vec3(u_mv * a_position);

	// Calculate the view-space normal
	vecN = normalize(mat3(u_mv) * a_normal);
	
	// Calculate the view-space light direction
	vec3 l_vecLight = vec3(mat3(u_mv) * u_lightPosition) ;
	vecL = normalize(normalize(l_vecLight) - v_eye);

	vecV = -normalize(v_eye);
	
	
	gl_Position = u_mvp * a_position;
	
	vert_pos = a_position.xyz;
	vert_uv = vec3(a_uv.x, 1.0 - a_uv.y, 0.0);
	
	vecT = normalize(mat3(u_mv) * a_tangent);
	vecBT = normalize(mat3(u_mv) * a_bitangent);
}
