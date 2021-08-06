// Fragment shader
#version 150

// UNIFORMS
uniform vec3 u_lightColor;

uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specularColor;
uniform float u_specularPower;

uniform sampler2D u_albedoTex;
uniform sampler2D u_normalMap;
uniform sampler2D u_metalMap;
uniform sampler2D u_glossMap;
uniform sampler2D u_ambientMap;
uniform samplerCube u_cubemap;

uniform int u_useAmbient;
uniform int u_useDiffuse;
uniform int u_useSpecular;
uniform int u_useAlbedoTex;
uniform int u_useNormalMap;
uniform int u_usePBR;
uniform int u_useAmbMap;
uniform int u_useEnvMap;
uniform int u_showNormals;
uniform int u_useGammaCorrec;
	
// INPUT	
in vec3 vecN;
in vec3 vecL;
in vec3 vecV;
in vec3 vecT;
in vec3 vecBT;
in vec3 vert_pos;
in vec3 vert_uv;

// OUTPUT
out vec4 frag_color;



vec3 ambient_reflection(in vec3 _N, in vec3 _V, in float _specularPower, in samplerCube _cubemap, in float _maxLevel)
{
    vec3 R = reflect(-_V, _N);
    float gloss = log2(_specularPower) / 10.0;
    float level = clamp(1.0 - gloss, 0.0, 1.0) * _maxLevel;
	return texture(_cubemap, R).rgb;
 //   return textureLod(cubemap, R, level).rgb;
}

float specular_normalized(in vec3 _N, in vec3 _H, in float _specularPower)
{
    float normalization = (8.0 + _specularPower) / 8.0;
    float specular = normalization * pow(max(0.0, dot(_N, _H)), _specularPower); // add max to remove black artifacts
	specular = min(1.0f, specular); // make sure max specular value is 1
	return specular;
}

float diffuse(in vec3 _N, in vec3 _L)
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
		color = vec4(0.5 * l_vecN + 0.5, 1.0);
	}
	else
	{
		// GET TEXTURE COLOR
		vec3 tex_col;
		if(u_useAlbedoTex == 1)
		{
			tex_col = texture(u_albedoTex, vert_uv.xy).rgb;
		}
		
		float spec_fact;
		float gloss_fact;
		if(u_usePBR == 1)
		{
			// GET SPECULAR FACTOR FROM METALLIC TEXTURE
			spec_fact = texture(u_metalMap, vert_uv.xy).r;
			
			// GET GLOSS FACTOR FROM GLOSSINESS TEXTURE
			float rough_fact = texture(u_glossMap, vert_uv.xy).r;
			gloss_fact = 1.0f - rough_fact;
		}
		
		float ambOcc;
		if(u_useAmbMap == 1)
		{
			// GET AMBIENT OCCLUSION FACTOR FROM AMBIENT OCCLUSION MAP
			ambOcc = texture(u_ambientMap, vert_uv.xy).r;
		}
		
	
		// -- Render Blinn-Phong shading --
		
		vec3 l_vecH = normalize(l_vecL + l_vecV);
	
		//DIFFUSE
		float diffuse = diffuse(l_vecN, l_vecL);
		if(u_useDiffuse == 1)
		{
			if(u_useAlbedoTex == 1)
			{
				color.rgb += tex_col * u_lightColor * diffuse;
			}
			else
			{
				color.rgb += u_diffuseColor * u_lightColor * diffuse;
			}
		}

		
		//SPECULAR
		if(u_useSpecular == 1)
		{
			float specular;
			if(u_usePBR == 1)
			{
				float specular_power = pow(2.0f, 8.0f*gloss_fact);
				specular = specular_normalized(l_vecN, l_vecH, gloss_fact*u_specularPower);
				specular *= spec_fact;
			}
			else
			{
				specular = specular_normalized(l_vecN, l_vecH, u_specularPower);
			}
			color.rgb += u_specularColor * u_lightColor * specular;
		}
		
		//AMBIENT OCCLUSION
		if(u_useAmbMap == 1)
		{
			color.rgb *= ambOcc;
		}
		
		//AMBIENT
		if(u_useAmbient == 1)
		{
			if(u_useAlbedoTex == 1)
			{
				color.rgb += tex_col * 0.05f;
			}
			else
			{
				color.rgb += u_ambientColor;
			}
		}

		//ENV MAP
		if(u_useEnvMap == 1)
		{
			if(u_usePBR == 1)
			{
				color.rgb += spec_fact * u_lightColor * ambient_reflection(l_vecN, l_vecV, u_specularPower, u_cubemap, 7);
			}
			else
			{
				color.rgb += u_lightColor * ambient_reflection(l_vecN, l_vecV, u_specularPower, u_cubemap, 7);
			}
		}
		
	
	}
	
	//GAMMA CORRECTION
	if(u_useGammaCorrec == 1)
	{
		color.rgb = linear_to_gamma(color.rgb);
	}
	
	frag_color = color;

}

