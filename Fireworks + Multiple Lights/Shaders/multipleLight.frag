#version 330 core

// Some drivers require the following
precision highp float;

struct materialStruct
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform int currentLights;
uniform int deletedLight;

#define NR_POINT_LIGHTS 19

in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform materialStruct material;
uniform sampler2D textureUnit0;

uniform float attConst;
uniform float attLinear;
uniform float attQuadratic;

uniform int boolLights;

in vec3 ex_V;
in vec2 ex_TexCoord;
in float ex_D;

layout(location = 0) out vec4 out_Color;

//Function prototypes
vec3 CalcDirLight(DirLight light, materialStruct mat, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, materialStruct mat, vec3 normal, vec3 fragPos, vec3 viewDir);
 
void main()
{    
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(ex_V - Position);
    
    // == ======================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == ======================================

    // Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, material, norm, viewDir);
   
    // Phase 2: Point lights
	if(boolLights == 1){
		for(int i = 0; i < currentLights; i++){
				result += CalcPointLight(pointLights[i], material, norm, Position, viewDir);   
		}
   }

    // Phase 3: Spot light
    // result += CalcSpotLight(spotLight, material, norm, Position, viewDir);    
    
    //vec4 color = vec4(result, 1.0);
	//out_Color = color;

	vec4 tmp_Color = (vec4(result,1.0)) * texture(textureUnit0, TexCoords);
	//out_Color = vec4(tmp_Color.rgb / (attConst + attLinear * ex_D + attQuadratic * ex_D*ex_D), tmp_Color.a);

	out_Color = vec4(tmp_Color.rgb / (0.4 + 0.7 * ex_D + 0.0 * ex_D*ex_D), tmp_Color.a);
	
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, materialStruct mat, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
   
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
   
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
   
    // Combine results
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, materialStruct mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
   
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
   
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
   
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
   
    // Combine results
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
   
    return (ambient + diffuse + specular);
}