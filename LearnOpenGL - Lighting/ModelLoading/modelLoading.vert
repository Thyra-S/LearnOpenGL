#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 1. Pass texture coordinates directly
    TexCoords = aTexCoords;    
    
    // 2. Calculate the position of the vertex in actual World Space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // 3. Pass the normals (using a Normal Matrix to prevent them from breaking if you scale your object)
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    // 4. Final screen position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}