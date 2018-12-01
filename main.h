#pragma once
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


#include <GL/glew.h>
#include <GL/freeglut.h>
//#include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 

using namespace std;
using namespace glm;

// Size of the terrain
const int MAP_SIZE = 65;

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

float camPhi = 0.6f;
float camTheta = 0;
vec3 los = {0, 0, 0};
vec3 eye = { 0,0,MAP_SIZE};
float speed = 0.2f;
float zoom = MAP_SIZE;
struct Vertex
{
	float coords[4];
	float colors[4];
};

static mat4 projMat = mat4(1.0);

static enum buffer
{
	TERRAIN_VERTICES
};
static enum object
{
	TERRAIN
};

// Globals
static Vertex terrainVertices[MAP_SIZE*MAP_SIZE] = {};

const int numStripsRequired = MAP_SIZE - 1;
const int verticesPerStrip = 2 * MAP_SIZE;

unsigned int terrainIndexData[numStripsRequired][verticesPerStrip];

static unsigned int
programId,
vertexShaderId,
fragmentShaderId,
modelViewMatLoc,
projMatLoc,
buffer[1],
vao[1];

void calculateView();
