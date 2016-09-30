#version 420

#define PI 3.14159265358979323846

// Inputs
in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos; 
in vec4 FragPosLightSpace;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  

struct Light {
    vec3 position;
	vec3 color;
};

uniform vec4 baseColor; 
uniform vec3 camPos;
uniform Light light[100];
uniform float metallness; 
uniform float roughness;
uniform Material material;
uniform int numLights;
uniform sampler2D material_diffuse;

uniform bool is_selected;
uniform bool is_valid;
uniform bool is_light;
uniform bool has_texture;

vec3 fNormal;
vec3 V;
vec3 L;
vec3 halfVector;
float NoV;
float NoH;
float NoL;
float VoH;

out vec4 finalColor; // output color

// Specular Functions
vec3 D(vec3 h)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float finalTerm = ((NoH * NoH) * (alpha2 - 1.0) + 1.0);
    return vec3(alpha2 / (PI * (finalTerm * finalTerm)));
}

vec3 Gsub(vec3 v)
{
    float k = ((roughness + 1.0) * (roughness + 1.0)) / 8.0;
	return vec3(NoV / ((NoV) * (1.0 - k) + k));
}

vec3 G(vec3 l, vec3 v, vec3 h) 
{
	return Gsub(l) * Gsub(v);
}

vec3 F(vec3 v, vec3 h)
{
	vec3 f0 = vec3(1,1,1);
	return f0 + (1.0 - f0) * pow(2, (-5.55473 * (VoH) - 6.98316) * (VoH));
}

//Cook-Torrance Model
vec3 specular()
{
    return (D(halfVector) * F(V, halfVector) * G(L, V, halfVector)) / 4.0 * (NoL * NoV);
}

vec3 diffuse()
{
   vec3 result = vec3(baseColor / PI);
   return result * NoL;
}

void main()
{
	vec3 final_diffuse;
	vec3 final_specular;

	for(int i = 0; i < numLights; i++)
	{
		fNormal = normalize(Normal);

		V = normalize(camPos - FragPos);
		L = normalize(light[i].position - FragPos);

		halfVector = normalize(L + V);

		NoV = max(dot(fNormal, V), 0.0);
		NoH = max(dot(fNormal, halfVector), 0.0);
		NoL = max(dot(fNormal, L), 0.0);
		VoH = max(dot(V, halfVector), 0.0);
	
		if (NoL > 1.0f)
		{
			NoL = 1.0f;
		}

		if (NoH > 1.0f)
		{
			NoH = 1.0f;
		}

		if (NoV > 1.0f)
		{
			NoV = 1.0f;
		}

		if (VoH > 1.0f)
		{
			VoH = 1.0f;
		}
	
		final_diffuse += diffuse() * light[i].color;
		final_specular += specular() * light[i].color;
	}

	if(has_texture)
	{
		vec3 textureColor = vec3(texture(material_diffuse, TexCoords));
		final_diffuse = 2 * textureColor * final_diffuse;
	}
	else
	{
		final_diffuse *= vec3(baseColor);
	}
		
    finalColor = vec4(final_diffuse + final_specular, 1.0);
	
	if(is_light)
	{
		finalColor = vec4(vec3(baseColor), 1.0f);
	}

	if(!is_valid)
	{
		finalColor = mix(finalColor, vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.2f);
	}
	if(is_selected)
	{
		finalColor = mix(finalColor, vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.2f);
	}
}
