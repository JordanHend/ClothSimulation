#version 330 core

out vec4 FragColor;
uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform bool renderNormal;
in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

vec3 lightdirection = vec3(1, -1, 0);
vec3 ambient = vec3(0.1, 0.1, 0.1);
vec3 diffuse = vec3(0.9, 0.9, 0.9);
vec3 specular = vec3(0.2, 0.2, 0.2);

void main()
{
  	vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-lightdirection);
 	vec3 viewDir = normalize(viewPos - FragPos);
 	vec3 color = vec3(0.5, 0, 0.2);


    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading

        vec3 halfwayDir = normalize(lightDir + viewDir);  
       float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

    // combine results
    vec3 ambient = ambient * color;
    vec3 diffuse = diffuse * diff * color;
    vec3 specular = specular * spec * color;
    vec3 outColor = (ambient + diffuse + specular);
	


if(renderNormal)
    FragColor = vec4(normal, 1.0);
else
	FragColor = vec4(outColor, 1.0);

}
