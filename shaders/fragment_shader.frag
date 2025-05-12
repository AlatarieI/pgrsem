#version 330 core

#define NR_POINT_LIGHTS 8

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct MaterialTexture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
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

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Material material;
uniform MaterialTexture materialTexture1;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform int activePointLights;
uniform int activeSpotLights;

uniform vec3 viewPos;
uniform vec3 colori;

uniform bool useDiffuseTexture;
uniform bool useSpecularTexture;
uniform bool useGammaCorrection;


uniform sampler2D fogTex;
uniform bool useFog;
uniform float time;


vec3 objectDiffuse;
vec3 objectSpecular;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 applyFog(vec3 fragColor, vec3 fogColor);

void main() {
    if (useDiffuseTexture) {
        objectDiffuse = texture(materialTexture1.diffuse, TexCoords).rgb;
    } else {
        objectDiffuse = material.diffuse;
    }

    if (useSpecularTexture) {
        objectSpecular = texture(materialTexture1.specular, TexCoords).rgb;
    } else {
        objectSpecular = material.specular;
    }


    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos-FragPos);


    // Directional lighting
    vec3 result = vec3(0.0f);
    result += CalcDirLight(dirLight, norm, viewDir);
    // Point lights
    if (activePointLights > 0) {
        for (int i = 0; i < activePointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
//     Spot light
    if (activeSpotLights > 0)
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    if (useGammaCorrection)
        result = pow(result,vec3(1/2.2));

    if (useFog)
        FragColor = applyFog(result, vec3(0.6, 0.7, 0.75));
    else
        FragColor = vec4(result, 1.0f);

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.direction);

    //ambient
    vec3 ambient = light.ambient * objectDiffuse;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * objectDiffuse;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * objectSpecular;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // ambient
    vec3 ambient  = light.ambient  * objectDiffuse;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse  * diff * objectDiffuse;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * objectSpecular;

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // ambient
    vec3 ambient = light.ambient * objectDiffuse;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * objectDiffuse;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * objectSpecular;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

vec4 applyFog(vec3 fragColor, vec3 fogColor) {
    float distance = length(FragPos - viewPos);

    float expFog = 1.0 - exp(-pow(0.05 * distance, 2.0));

    vec2 uv = FragPos.xz * 0.007 + vec2(time * 0.01, 0.0);
    uv = uv - vec2(0.0f, 0.5f);
    float textureFog = texture(fogTex, uv).r;

    float fogAmount = min(1.0, expFog + textureFog);

    return vec4(mix(fragColor, fogColor, fogAmount),1.0);
}