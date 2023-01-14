#version 330 core

in vec3 iPosition;
in vec3 iNormal;
in vec2 iTexCoord;

uniform vec3 cameraPos;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform bool hadMapKa;
uniform sampler2D mapKa;
uniform bool hadMapKd;
uniform sampler2D mapKd;
uniform bool hadMapKs;
uniform sampler2D mapKs;
uniform bool hadMapNs;
uniform sampler2D mapNs;

uniform vec3 ambientLight;
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;

uniform vec3 spotLightPos;
uniform vec3 spotLightIntensity;
uniform vec3 spotLightDir;
uniform float cutoffStart;
uniform float totalWidth;

uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;

out vec4 FragColor;

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir, bool hadMapKd);
vec3 Specular(vec3 Ks, vec3 I, vec3 viewDir, vec3 reflectDir, float Ns);
vec3 PointLight(vec3 pointLightPos, vec3 position, vec3 normal, vec3 viewDir);
vec3 SpotLight(vec3 spotLightPos, vec3 position, vec3 normal, vec3 viewDir);
vec3 DirLight(vec3 dirLightDir, vec3 normal, vec3 viewDir);


vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir, bool hadMapKd)
{
    vec3 KdColor = Kd;
    if (hadMapKd)
        KdColor = vec3(texture2D(mapKd, iTexCoord));
    return KdColor * I * max(dot(N, lightDir), 0.0);
}

vec3 Specular(vec3 Ks, vec3 I, vec3 viewDir, vec3 reflectDir, float Ns)
{
    vec3 KsColor = Ks;
    float NsVal = Ns;
    if(hadMapKs)
        KsColor = vec3(texture2D(mapKs, iTexCoord));
    if(hadMapNs)
        NsVal = float(texture2D(mapNs, iTexCoord));
    return KsColor * I * pow(max(dot(viewDir, reflectDir), 0.0), NsVal);
}

vec3 PointLight(vec3 pointLightPos, vec3 position, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLightPos - position);
    vec3 reflectDir = normalize(reflect(-lightDir, normal));

    float distance = length(pointLightPos - position);
    float attenuation = 1.0 / (distance * distance);
    vec3 intensity = pointLightIntensity * attenuation;
    
    vec3 diffuse = Diffuse(Kd, intensity, normal, lightDir, hadMapKd);
    vec3 specular = Specular(Ks, intensity, viewDir, reflectDir, Ns);
    return diffuse + specular;
}

vec3 SpotLight(vec3 spotLightPos, vec3 position, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(spotLightPos - position);
    vec3 reflectDir = normalize(reflect(-lightDir, normal));

    float cosTheta = dot(lightDir, normalize(-spotLightDir));
    float epsilon = cos(radians(cutoffStart)) - cos(radians(totalWidth));

    float distance = length(spotLightPos - position);
    float attenuation = 1.0 / (distance * distance);
    vec3 intensity = spotLightIntensity * clamp((cosTheta - cos(radians(totalWidth))) / epsilon, 0.0, 1.0)  * attenuation;

    vec3 diffuse = Diffuse(Kd, intensity, normal, lightDir, hadMapKd);
    vec3 specular = Specular(Ks, intensity, viewDir, reflectDir, Ns);
    return diffuse + specular;
}

vec3 DirLight(vec3 dirLightDir, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLightDir);
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    
    vec3 diffuse = Diffuse(Kd, dirLightRadiance, normal, lightDir, hadMapKd);
    vec3 specular = Specular(Ks, dirLightRadiance, viewDir, reflectDir, Ns);
    return diffuse + specular;
}

void main()
{
    vec3 normal = normalize(iNormal);
    vec3 viewDir = normalize(cameraPos - iPosition);
    vec3 iColor = Ka * ambientLight;
    if(hadMapKa)
        iColor = vec3(texture2D(mapKs, iTexCoord)) * ambientLight;

    iColor += PointLight(pointLightPos, iPosition, normal, viewDir);
    iColor += SpotLight(spotLightPos, iPosition, normal, viewDir);
    iColor += DirLight(dirLightDir, normal, viewDir);
    FragColor =  vec4(iColor, 1.0);
}
