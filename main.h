#pragma once
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "Lighting.h"
#include <vector>

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

struct Quad
{
	glm::vec3 t0;	glm::vec3 t1;
};

Quad quadNormals[MAP_SIZE - 1][MAP_SIZE - 1];
glm::vec3 vertNormals[MAP_SIZE][MAP_SIZE];
static mat3 normalMat = mat3(1.0);

struct Vertex
{
	float coords[4];
	float normals[3];
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
normalMatLoc,
projMatLoc,
buffer[1],
vao[1];




static const Material groundTerrainFAndB = Material(
glm::vec4(0),
glm::vec4(1, 1, 1, 1),
glm::vec4(0),
glm::vec4(0,0,0,1),
50.0f );

 
static const glm::vec4 globalAmbientLight = vec4(0.2, 0.2, 0.2, 1.0);

static const Light mainLight =
{
	glm::vec4(0,0,0,1),
	glm::vec4(1,1,1,1),
	glm::vec4(1,1,1,1),
	glm::vec4(1,1,0,0)
};

char * readTextFile(char * aTextFile);

void shaderCompileTest(GLuint shader);

void openGLConfigure();

Quad createQuadNormals(int x, int y, glm::vec3  points[MAP_SIZE][MAP_SIZE]);



void calcNormals(int g, int h);

void setup(void);

void drawScene(void);

void resize(int w, int h);

void calculateView();
void keyInput(unsigned char key, int x, int y);
void specialKeyInput(int key, int x, int y);
void mouseKeyInput(int button, int state, int x, int y);

int main(int argc, char * argv[]);