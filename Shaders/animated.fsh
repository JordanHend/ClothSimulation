#version 330 core

out vec4 FragColor;
uniform sampler2D texture_diffuse1;


uniform float far_plane;
in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;
in float visibility; 

void main()
{
  
  

    vec4 outColor =texture(texture_diffuse1, TexCoords);
    FragColor = vec4(outColor);
   

}
