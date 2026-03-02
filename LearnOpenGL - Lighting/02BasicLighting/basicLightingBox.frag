#version 460 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor1;
uniform vec3 lightColor2;
uniform vec3 lightPos1; 
uniform vec3 lightPos2; 
uniform vec3 viewPos;

in vec3 Normal; 
in vec3 FragPos;  

void main()
{
    // Light 1
    // Ambient lighting
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * (lightColor1 + lightColor2);

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos1 - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor1;

    // Specular lighting    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor1;   

    

    // Light 2
    // Ambient lighting

    // Diffuse lighting
    lightDir = normalize(lightPos2 - FragPos); 
    diff = max(dot(norm, lightDir), 0.0);
    diffuse += diff * lightColor2;

    // Specular lighting    
    reflectDir = reflect(-lightDir, norm);  
    spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    specular += specularStrength * spec * lightColor2;   

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
    
}