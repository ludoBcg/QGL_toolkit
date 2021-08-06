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

uniform mat4 u_mv;
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
uniform vec3 u_lightPosition;
	
// INPUT	
in vec3 vecN;
in vec3 vecV;
in vec3 vecT;
in vec3 vecBT;
in vec3 vert_pos;
in vec3 vert_uv;
in vec3 v_eye;

// OUTPUT
out vec4 frag_color;

const float PI = 3.14159265359;


vec3 ambient_reflection(in vec3 _N, in vec3 _V, in float _specularPower, in samplerCube _cubemap, in float _maxLevel)
{
    vec3 R = reflect(-_V, _N);
    float gloss = log2(_specularPower) / 10.0;
    float level = clamp(1.0 - gloss, 0.0, 1.0) * _maxLevel;
	return texture(_cubemap, R).rgb;
}


// Normal distribution function (D)
float DistributionGGX(vec3 _N, vec3 _H, float _a)
{
	// Compute NDF using Trowbridge-Reitz GGX
	// (statistically approximate the general alignment of the microfacets given some roughness parameter)
	
	float a2     = _a*_a;
	float NdotH  = max(dot(_N, _H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom    = a2;
	float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
	denom        = PI * denom * denom;

	return nom / denom;
}

// Geometry function (G)
float GeometrySchlickGGX(float _NdotV, float _k)
{
    float nom   = _NdotV;
    float denom = _NdotV * (1.0 - _k) + _k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 _N, vec3 _V, vec3 _L, float _k)
{
	// Compute geometry function using Schlick-GGX
	// (statistically approximates the relative surface area where its micro surface-details overshadow each other causing light rays to be occluded)

    float NdotV = max(dot(_N, _V), 0.0);
    float NdotL = max(dot(_N, _L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, _k);
    float ggx2 = GeometrySchlickGGX(NdotL, _k);
	
    return ggx1 * ggx2;
}

// Fresnel equation (F)
vec3 fresnelSchlick(float _cosTheta, vec3 _F0)
{
	// Fresnel-Schlick approximation for F
	// (describes the ratio of light that gets reflected over the light that gets refracted)
	
    return _F0 + (1.0 - _F0) * pow(1.0 - _cosTheta, 5.0);
}

vec3 linear_to_gamma(in vec3 _color)
{
    return pow(_color, vec3(1.0f / 2.2f));
}

// MAIN
// see https://learnopengl.com/PBR/Lighting
void main()
{
	vec3 l_vecN;	// Normal vector
	vec3 l_vecV;	// View vector	
	vec3 l_vecL;	// Light vector
	
	// Calculate the view-space light direction (Light vector)
	vec3 l_vecLight = vec3(mat3(u_mv) * u_lightPosition) ;
	vec3 vecL = normalize(normalize(l_vecLight) - v_eye);
	
	// 1- Get input vectors ------------------------------------------------

	if(u_useNormalMap == 1)
	{
		// Read new normal from normal map
		l_vecN = texture(u_normalMap, vert_uv.xy).rgb * 2.0 - 1.0;
		l_vecN = normalize(l_vecN);
		
		// compute TBN matrix
		mat3 TBN = transpose( mat3(vecT, vecBT, vecN) );
		// compute new version of L and V in tangent space
		l_vecL = normalize( TBN * vecL);
		l_vecV = normalize( TBN * vecV );
	}
	else
	{
		l_vecN = vecN;
		l_vecL = vecL;
		l_vecV = vecV;
	}
	
	// 2- Compute shading -------------------------------------------------

	// final color
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	if(u_showNormals == 1)
	{
		// Render normals
		color = vec4(0.5 * l_vecN + 0.5, 1.0);
	}
	else
	{
		// 2.1- Get albedo color -------------------------------------------
		
		// albedo map
		vec3 albedoD;
		vec3 albedoS;
		if(u_useAlbedoTex == 1)
		{
			albedoD = texture(u_albedoTex, vert_uv.xy).rgb;
			albedoS = albedoD;
		}
		else
		{
			albedoD = u_diffuseColor;
			albedoS = u_specularColor;
		}
		// env map
		if(u_useEnvMap == 1)
		{
			albedoS +=  ambient_reflection(l_vecN, l_vecV, u_specularPower, u_cubemap, 7);
		}
		
		// ambient
		float ambOcc = 1.0;
		float ambFact = 0.0;
		if(u_useAmbient == 1)
		{
			ambFact = 1.0;
		}
		if(u_useAmbMap == 1)
		{
			ambOcc = texture(u_ambientMap, vert_uv.xy).r;
		}
		
	
		// 2.2- Get metalness and roughness coeffs  ------------------------
		
		float metalness;
		float roughness;
		float glossiness;
		if(u_usePBR == 1)
		{
			// Get metallic factor from metallicness texture
			metalness = texture(u_metalMap, vert_uv.xy).r;
			
			// Get gloss factor from glossiness texture
			roughness = texture(u_glossMap, vert_uv.xy).r;
			glossiness = 1.0f - roughness;
		}
		else
		{
			metalness = 0.5f;
			roughness = 1.0f - (u_specularPower / 2048.0f);
		}
		
		
		// 2.3- Compute Cook Torrance BRDF (f_r) ---------------------------
		
		// base reflectivity of the surface
		vec3 F0 = vec3(0.04);
		F0 = mix(F0, albedoS.rgb, metalness);
		
		// START for each light source (only one here)
		// [recompute l_vecL for currence light source]
		
		// Halfway vector
		vec3 l_vecH = normalize(l_vecL + l_vecV);
		

		// Cook Torrance BRDF
		vec3 f_r = vec3(0.0);
		
		// specular term
		
	    float D = DistributionGGX(l_vecN, l_vecH, roughness);
		float G = GeometrySmith(l_vecN, l_vecV, l_vecL, roughness);
		vec3 F = fresnelSchlick(max(dot(l_vecH, l_vecV), 0.0), F0);
 
		vec3 numerator = D * F * G;
		float denominator = 4.0 * max(dot(l_vecN, l_vecV), 0.0) * max(dot(l_vecN, l_vecL), 0.0);
	
		vec3 f_CookTorrance = numerator / max(denominator, 0.001);


		// Lambertian diffuse term
		
		vec3 f_Lambert = albedoD / PI;
		

		vec3 k_s = F;
		vec3 k_d = vec3(1.0) - k_s/*f_CookTorrance*/;
		k_d *= 1.0 - metalness;
		

		if(u_useDiffuse == 1)
		{
			f_r += k_d * f_Lambert;
		}
		
		if(u_useSpecular == 1)
		{
			f_r += f_CookTorrance;
		}
		
		
		// 2.3- Compute reflectance equation -------------------------------

		vec3 Lo = vec3(0.0);
		float distance = length(normalize(l_vecLight) - v_eye );
		distance *= 0.15; // reduce distance to reduce attenuation
		
		float attenuation = 1.0 / (distance * distance);

		//attenuation = 10;
		vec3 radiance     = u_lightColor * attenuation;
		// add to outgoing radiance Lo
		float NdotL = max(dot(l_vecN, l_vecL), 0.0);                
		Lo += f_r * radiance * NdotL; 
	
		// END for each light source
		

		// 2.4- Compute ambient --------------------------------------------
		
		vec3 ambient = vec3(0.03) * albedoD * ambFact;
		color.rgb = ambient + Lo;
		if(u_useAmbMap == 1) 
		{
			color.rgb = ambOcc * Lo;
		}

	}
	
	//GAMMA CORRECTION
	if(u_useGammaCorrec == 1)
	{
		color.rgb = linear_to_gamma(color.rgb);
	}
	
	frag_color = color;

}