#version 330 core
out vec4 FragColor;
uniform vec3 colorPlaneta;

void main()
{
    FragColor = vec4(colorPlaneta, 1.0f);
}