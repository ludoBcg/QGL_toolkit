// Fragment shader
#version 150

// UNIFORMS
uniform vec3 u_lightColor;

uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specularColor;
uniform float u_specularPower;

uniform sampler2D u_tex;
uniform sampler2D u_normalMap;

uniform int u_useAmbient;
uniform int u_useDiffuse;
uniform int u_useSpecular;
uniform int u_useTex;
uniform int u_useNormalMap;
uniform int u_showNormals;
uniform int u_useGammaCorrec;
uniform int u_useMeshCol;
	
// INPUT	
in vec3 vecN;
in vec3 vecL;
in vec3 vecV;
in vec3 vecT;
in vec3 vecBT;
in vec3 vert_pos;
in vec3 vert_uv;
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

	
	
	vec3 l_vecN;
	vec3 l_vecL;
	vec3 l_vecV;
	
	vec3 l_modelN = modelN;
	
	if(u_useNormalMap == 1)
	{
		// Read new normal from normal map
		l_vecN = texture(u_normalMap, vert_uv.xy).rgb * 2.0 - 1.0;
		l_vecN = normalize(l_vecN);
		
		// compute TBN matrix
		mat3 TBN = transpose( mat3(vecT, vecBT, vecN) );
		// compute new version of L and V in tangent space
		l_vecL = normalize( TBN * vecL );
		l_vecV = normalize( TBN * vecV );
		
		l_modelN = l_vecN;
	
	}
	else
	{
		l_vecN = vecN;
		l_vecL = vecL;
		l_vecV = vecV;
	}
	
	// final color
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	if(u_showNormals == 1)
	{
		// -- Render normals --
		color = vec4(0.5 * l_modelN + 0.5, 1.0);
	}
	else
	{
		vec3 diff_col = u_diffuseColor;
		
		
		// GET MESH COLOR
		if(u_useMeshCol == 1)
		{
			diff_col = col;
		}
		
		// GET TEXTURE COLOR
		if(u_useTex == 1)
		{
			diff_col = texture(u_tex, vert_uv.xy).rgb;
		}
		
		
	
		// -- Render Blinn-Phong shading --
		
		vec3 l_vecH = normalize(l_vecL + l_vecV);
	
		//DIFFUSE
		float diffuse = compDiff(l_vecN, l_vecL);
		if(u_useDiffuse == 1)
		{
			color.rgb += diff_col * u_lightColor * diffuse;
		}

		
		//SPECULAR
		if(u_useSpecular == 1)
		{
			float specular = specular_normalized(l_vecN, l_vecH, u_specularPower);

			color.rgb += u_specularColor * u_lightColor * specular;
		}
		
		
		//AMBIENT
		if(u_useAmbient == 1)
		{
			if(u_useTex == 1 || u_useMeshCol == 1)
			{
				color.rgb += diff_col * 0.05f;
			}
			else
			{
				color.rgb += u_ambientColor;
			}
		}

	
	}
	
	//GAMMA CORRECTION
	if(u_useGammaCorrec == 1)
	{
		color.rgb = linear_to_gamma(color.rgb);
	}
	
	frag_color = color;
	//frag_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);

}

