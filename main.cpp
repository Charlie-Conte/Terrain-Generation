#include "main.h"

// Function to read text file, used to read shader files
char* readTextFile(char* aTextFile)
{
	FILE* filePointer = fopen(aTextFile, "rb");
	char* content = NULL;
	long numVal = 0;

	fseek(filePointer, 0L, SEEK_END);
	numVal = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	content = (char*)malloc((numVal + 1) * sizeof(char));
	fread(content, 1, numVal, filePointer);
	content[numVal] = '\0';
	fclose(filePointer);
	return content;
}

void shaderCompileTest(GLuint shader)
{
	GLint testResult = GL_FALSE;
	int testLogLength;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &testResult);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &testLogLength);
	std::vector<GLchar> vertShaderTestError((testLogLength > 1) ? testLogLength : 1);
	glGetShaderInfoLog(shader, testLogLength, NULL, &vertShaderTestError[0]);
	std::cout << &vertShaderTestError[0] << std::endl;
}


void openGLConfigure()
{
	glClearColor(1.0, 1.0, 0.9, 0.0);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	// Create shader program executable - read, compile and link shaders
	//Vertex Shader
	char* vertexShader = readTextFile("vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);
	//Test for vertex shader compilation errors
	std::cout << "VERTEX::" << std::endl;
	shaderCompileTest(vertexShaderId);

	//Fragment Shader
	char* fragmentShader = readTextFile("fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	//Test for fragment shader compilation errors
	std::cout << "FRAGMENT::" << std::endl;
	shaderCompileTest(fragmentShaderId);

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);
	std::cout << "COMPUTE::" << std::endl;
	std::cout << glGetError() << std::endl;
	///////////////////////////////////////

	//  set the material uniform locations so that they can be accessed by the shader
	glUniform4fv(glGetUniformLocation(programId, "terrainFAndB.ambiReflect"), 1,
				 &groundTerrainFAndB.ambiReflect[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFAndB.diffReflect"), 1,
				 &groundTerrainFAndB.diffReflect[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFAndB.specReflect"), 1,
				 &groundTerrainFAndB.specReflect[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFAndB.emitColours"), 1,
				 &groundTerrainFAndB.emitColours[0]);
	glUniform1f(glGetUniformLocation(programId, "terrainFAndB.shininess"),
				groundTerrainFAndB.shininess);

	glUniform4fv(glGetUniformLocation(programId, "globalAmbientLight"), 1, &globalAmbientLight[0]);

	///////////////////////////////////////

	// set the directional light uniform locations so that they can be accessed by the shader
	glUniform4fv(glGetUniformLocation(programId, "mainLight.ambiColours"), 1,
				 &mainLight.ambiColours[0]);
	glUniform4fv(glGetUniformLocation(programId, "mainLight.diffColours"), 1,
				 &mainLight.diffColours[0]);
	glUniform4fv(glGetUniformLocation(programId, "mainLight.specColours"), 1,
				 &mainLight.specColours[0]);
	glUniform4fv(glGetUniformLocation(programId, "mainLight.position"), 1,
				 &mainLight.position[0]);



	///////////////////////////////////////

	// Create vertex array object (VAO) and vertex buffer object (VBO) and associate terrain with vertex shader.
	glGenVertexArrays(1, vao);
	glGenBuffers(1, buffer);
	glBindVertexArray(vao[TERRAIN]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[TERRAIN_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (GLvoid*)(4 * sizeof GLfloat));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (GLvoid*)(7 * sizeof GLfloat));
	glEnableVertexAttribArray(2);
	normalMatLoc = glGetUniformLocation(programId, "normalMat");

	///////////////////////////////////////

	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(programId, "projMat");
	projMat = perspective(radians(60.0), (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, 0.1, 10000.0);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

	///////////////////////////////////////

	// Obtain modelview matrix uniform location and set value.
	mat4 modelViewMat = mat4(1.0);
	modelViewMatLoc = glGetUniformLocation(programId, "modelViewMat");
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

	///////////////////////////////////////

	// Calculate and update normal matrix, after any changes to the view matrix
	normalMat = transpose(inverse(mat3(modelViewMat)));
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));
	
}

Quad createQuadNormals(int x, int y, glm::vec3  points[MAP_SIZE][MAP_SIZE])
{
	Quad out;
	glm::vec3 t0p0 =  points[y][x - 1];
	glm::vec3 t0p1 = points[y][x];
	glm::vec3 t0p2 = points[y - 1][x - 1];

	glm::vec3 t1p0 = points[y][x];
	glm::vec3 t1p1 = points[y-1][x];
	glm::vec3 t1p2 = points[y - 1][x - 1];

	out.t0 += glm::cross(t0p0 - t0p1, t0p0 - t0p2);
	out.t0 += glm::cross(t0p1 - t0p2, t0p1 - t0p0);
	out.t0 += glm::cross(t0p2 - t0p0, t0p2 - t0p1);

	out.t1 += glm::cross(t0p0 - t0p1, t0p0 - t0p2);
	out.t1 += glm::cross(t0p1 - t0p2, t0p1 - t0p0);
	out.t1 += glm::cross(t0p2 - t0p0, t0p2 - t0p1);

	//glm::vec3 t0u = t0p1 - t0p0;
	//glm::vec3 t0v = t0p2 - t0p0;	

	//glm::vec3 t1u = t1p1 - t1p0;
	//glm::vec3 t1v = t1p2 - t1p0;

	//out.t0.x = (t0u.y*t0v.z) - (t0u.z*t0v.y);
	//out.t0.y = (t0u.z*t0v.x) - (t0u.x*t0v.z);
	//out.t0.z = (t0u.x*t0v.y) - (t0u.y*t0v.x);	
	//
	//out.t1.x = (t1u.y*t1v.z) - (t1u.z*t1v.y);
	//out.t1.y = (t1u.z*t1v.x) - (t1u.x*t1v.z);
	//out.t1.z = (t1u.x*t1v.y) - (t1u.y*t1v.x);
	return out;
}

void calcNormals(int y, int x)
{
	glm::vec3 sum = glm::vec3(0.f);
	if (x != 0 && y != MAP_SIZE - 1)
		sum += quadNormals[y][x - 1].t1;

	if (x != MAP_SIZE - 1 && y != MAP_SIZE - 1)
		sum += quadNormals[y][x].t0 + quadNormals[y][x].t1;	
	
	if (x != MAP_SIZE - 1 && y != 0)
		sum += quadNormals[y - 1][x].t0;
	
	if (x != 0 && y != 0)
		sum += quadNormals[y - 1][x - 1].t0 + quadNormals[y - 1][x - 1].t1;

	vertNormals[y][x] = glm::normalize(sum);

	//vec3 normal = normalize(normalMat * vertNormals[y][x]);
	//vec3 lightDirection = normalize(vec3(mainLight.position));
	//vec4 a = (mainLight.diffColours * groundTerrainFAndB.diffReflect);
	//float d = dot(normal, lightDirection);
	//float b = max(d, 0.0f);
	//vec4 oout = b * a;
	//cout << oout.x<<", "<<oout.y<<", "<<oout.z<< endl;
}

// Initialization routine.
void setup(void)
{
	// Initialise terrain - set values in the height map to 0
	float terrain[MAP_SIZE][MAP_SIZE] = {};
	for (int x = 0; x < MAP_SIZE; x++)
	{
		for (int z = 0; z < MAP_SIZE; z++)
		{
			terrain[x][z] = 0;
		}
	}


	int itteration = 1;
	int stepSize = MAP_SIZE - 1;
	float randomMax = 0.3 * MAP_SIZE;
	float roughness = .002 * MAP_SIZE;

	terrain[0][0] = (linearRand(0.0f, randomMax) * 2 * roughness) - roughness;
	terrain[0][MAP_SIZE - 1] = (linearRand(0.0f, randomMax) * 2 * roughness) - roughness;
	terrain[MAP_SIZE - 1][0] = (linearRand(0.0f, randomMax) * 2 * roughness) - roughness;
	terrain[MAP_SIZE - 1][MAP_SIZE - 1] = (linearRand(0.0f, randomMax) * 2 * roughness) - roughness;

	while (stepSize >= 2.0f)
	{
		int hStepSize = stepSize / 2;
		//Square
		for (int x = 0; x < MAP_SIZE - 1; x += stepSize)
		{
			for (int y = 0; y < MAP_SIZE - 1; y += stepSize)
			{
				float avgHeight = ((
					terrain[x][y] +
					terrain[x + stepSize][y] +
					terrain[x][y + stepSize] +
					terrain[x + stepSize][y + stepSize]
					) / 4.0f) + (linearRand(0.0f, randomMax) * 2 * roughness) - roughness;
				terrain[x + hStepSize][y + hStepSize] = avgHeight + linearRand(-randomMax, randomMax);
				//cout << "(" << x << "," << y << ")\n";
			}
		}
		//Diamond
		for (int x = 0; x < MAP_SIZE - 1; x += hStepSize)
		{
			for (int y = (x + hStepSize) % stepSize; y < MAP_SIZE - 1; y += stepSize)
			{
				int sMO = MAP_SIZE - 1;
				float avgHeightRan =
					((
					terrain[(x - hStepSize + MAP_SIZE - 1) % sMO][y] +
					terrain[(x + hStepSize) % sMO][y] +
					terrain[x][(y - hStepSize + MAP_SIZE - 1) % sMO] +
					terrain[x][(y + hStepSize) % sMO]
					) / 4.0f) + (linearRand(-randomMax, randomMax) * 2 * roughness) - roughness;
				terrain[x][y] = avgHeightRan;
				if (x == 0) terrain[MAP_SIZE - 1][y] = avgHeightRan;
				if (y == 0) terrain[x][MAP_SIZE - 1] = avgHeightRan;
				//		cout << "(" << x << "," << y << ")\t\t" << "Height: " << avgHeightRan << endl;
			}
		}
		randomMax = randomMax / 2;
		stepSize = stepSize / 2;
		itteration++;
	}


	float MSmOdT = (MAP_SIZE - 1.0f) / 2.0f;
	glm::vec3 terrainPoints[MAP_SIZE][MAP_SIZE];

	// Intialise vertex array

	for (int z = 0; z < MAP_SIZE; z++)
	{
		
		for (int x = 0; x < MAP_SIZE; x++)
		{
			terrainPoints[z][x] = { MSmOdT - (float)x, terrain[x][z], MSmOdT - (float)z };
			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			
		}

	}
	

	for (int y = 1; y < MAP_SIZE; y++)
	{		
		for (int x = 1; x < MAP_SIZE; x++)
		{		
			quadNormals[y-1][x-1] = createQuadNormals(x, y, terrainPoints);
		}
	}

	int i = 0;
	for (int g = 0; g < MAP_SIZE; g++)
	{
		glm::vec4 col = glm::vec4(1);//glm::linearRand(0, 1), glm::linearRand(0, 1), glm::linearRand(0, 1), 1);
		for (int h = 0; h < MAP_SIZE; h++)
		{
			calcNormals(g, h);
			
			
			terrainVertices[i] = { {terrainPoints[g][h].x,terrainPoints[g][h].y,terrainPoints[g][h].z,1.0},
			{ vertNormals[g][h][0],vertNormals[g][h][1],vertNormals[g][h][2] },
			{ col[0],col[1],col[2],col[3] } };
			i++;
		}
	}


	//int midVert = (int)size(terrainVertices) / 2;
	//cout << midVert << endl;
	//cout << terrainVertices[midVert].coords[0] << ",\t" << terrainVertices[midVert].coords[1]<<",\t" << terrainVertices[midVert].coords[2] << endl;
	// Now build the index terrain 
	i = 0;
	for (int z = 0; z < MAP_SIZE - 1; z++)
	{
		i = z * MAP_SIZE;
		for (int x = 0; x < MAP_SIZE * 2; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
		for (int x = 1; x < MAP_SIZE * 2 + 1; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
	}

	openGLConfigure();
	calculateView();
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Obtain modelview matrix uniform location and set value.
	mat4 modelViewMat = mat4(1.0);
	// Move terrain into view - glm::translate replaces glTranslatef
	modelViewMat = lookAt(eye, los, vec3(0, 1, 0)); //translate(modelViewMat, vec3(-15.0f, -12.0f, -60)); // 5x5 grid



	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));


	// For each row - draw the triangle strip
	for (int i = 0; i < MAP_SIZE - 1; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, verticesPerStrip, GL_UNSIGNED_INT, terrainIndexData[i]);
	}

	glFlush();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
	case 'd':
		if (key == 'a')
		{
			if (los.x >= MAP_SIZE - speed)break;
			los.x += speed * MAP_SIZE;

		}
		else
		{
			if (los.x <= -MAP_SIZE + speed)break;
			los.x -= speed * MAP_SIZE;
		}
		glutPostRedisplay();
		break;
	case 'w':
	case 's':
		if (key == 'w')
		{
			if (los.z >= MAP_SIZE - speed)break;
			los.z += speed * MAP_SIZE;

		}
		else
		{
			if (los.z <= -MAP_SIZE + speed)break;
			los.z -= speed * MAP_SIZE;
		}
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;

	default:
		break;
	}
}
// Keyboard input processing routine.
void specialKeyInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
	case GLUT_KEY_LEFT:
		if (key == GLUT_KEY_RIGHT)
		{
			camTheta -= speed;
		}
		else
		{
			camTheta += speed;
		}

		calculateView();

		//los.x = cosf(camPhi) * sinf(camTheta);
		//los.y = sinf(camPhi);
		//los.z = cosf(camPhi) * -cosf(camTheta);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		if (key == GLUT_KEY_DOWN)
		{
			camPhi -= speed;
		}
		else
		{
			camPhi += speed;
		}
		//
		calculateView();
		//los.x = cosf(camPhi) * sinf(camTheta);
		//los.y = sinf(camPhi);
		//los.z = cosf(camPhi) * -cosf(camTheta);
		glutPostRedisplay();
		break;

	default:
		break;
	}
	//cout << los.x << ",\t" << los.y << ",\t" << los.z<< endl;
	//cout << eye.x << ",\t" << eye.y << ",\t" << eye.z<< endl;
	//cout << camPhi << endl;
}
void calculateView()
{
	eye.x = zoom * cos(camPhi) * sin(camTheta);
	eye.y = zoom * sin(camPhi);
	eye.z = zoom * cos(camPhi) * -cos(camTheta);


	//eye.x = zoom * cosf(camTheta);
	////eye.y = 0;
	//eye.z = zoom * sinf(camTheta);
	//los.x = -zoom * cosf(camTheta);
	////los.y = 0;
	//los.z = -zoom * sinf(camTheta);
}
void mouseKeyInput(int button, int state, int x, int y)
{
	switch (button)
	{

	case 3://wheel up
	case 4://wheel down
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		if (button == 4)
		{
			zoom = zoom / (speed * 4);
		}
		else
		{
			zoom = zoom * speed * 4;
		}
		cout << zoom << endl;
		//cout << "ex: " << eye.x << "\tez: " << eye.z << endl;
		calculateView();
		glutPostRedisplay();
		break;

	}

}


// Main routine.
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glEnable(GL_DEPTH_TEST);
	// Set the version of OpenGL (4.2)
	glutInitContextVersion(4, 2);
	// The core profile excludes all discarded features
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// Forward compatibility excludes features marked for deprecation ensuring compatability with future versions
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("TerrainGeneration");

	// Set OpenGL to render in wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);
	glutMouseFunc(mouseKeyInput);


	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
