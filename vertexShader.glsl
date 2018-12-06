#version 420 core


layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec3 terrainNorms;
layout(location=2) in vec4 terrainColors;

uniform mat4 projMat;
uniform mat4 modelViewMat;
uniform mat3 normalMat;

out vec3 normal;

void main(void)
{
   gl_Position = projMat * modelViewMat * terrainCoords;
   normal = normalize(normalMat * terrainNorms);
   //colorsExport = vec4(terrainNorms.x,terrainNorms.y,terrainNorms.z,0);
   
   //colorsExport = globalAmbientLight * terrainFAndB.ambiReflect;
}