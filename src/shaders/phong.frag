// Fragment shader
#version 150

// UNIFORMS
uniform vec3 u_lightColor;

uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specularColor;
uniform float u_specularPower;


// INPUT	
in vec3 vecN;
in vec3 vecL;
in vec3 vecV;
in vec3 col;
in vec3 modelN;

// OUTPUT
out vec4 frag_color;





float specular_normalized(in vec3 _N, in vec3 _H, in float _specularPower)
{
    float normalization = (8.0 + _specularPower) / 8.0;
    float specular = normalization * pow(max(0.0, dot(_N, _H)), _specularPower); // add max to remove black artifacts
	specular = min(1.0f, specular); // make sure max specular value is 1
	return specular;
}


float compDiff(in vec3 _N, in vec3 _L)
{
	// Calculate the diffuse (Lambertian) reflection term
    return max(0.0, dot(_N, _L));
}

vec3 linear_to_gamma(in vec3 _color)
{
    return pow(_color, vec3(1.0f / 2.2f));
}


// MAIN
void main()
{

	vec3 l_vecN = vecN;
	vec3 l_vecL = vecL;
	vec3 l_vecV = vecV;

	// final color
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	vec3 diff_col = u_diffuseColor;
	

	// -- Render Blinn-Phong shading --
	vec3 l_vecH = normalize(l_vecL + l_vecV);

	//DIFFUSE
	float diffuse = compDiff(l_vecN, l_vecL);
	color.rgb += diff_col * u_lightColor * diffuse;


	//SPECULAR
	float specular = specular_normalized(l_vecN, l_vecH, u_specularPower);
	color.rgb += u_specularColor * u_lightColor * specular;


	//AMBIENT
	color.rgb += u_ambientColor;

	//GAMMA CORRECTION
	color.rgb = linear_to_gamma(color.rgb);


	frag_color = color;

}

